#include "cpu.hpp"

CPU::CPU(const Program &prog, const Config &config) : 
	pc(prog.entry_point), end(prog.entry_point + prog.instrs.size()),
	mem(prog.instrs, MEM_SIZE), regs(NUM_REGISTERS, log),
	width(config.pipe_width), rob(NUM_REGISTERS * 2), rat(NUM_REGISTERS),
	branch_pred([config]() -> unique_ptr<BranchPredictor> {
		if (config.branch_pred == "static_taken") {
			return make_unique<StaticBranchPredictor>(true);
		} else if (config.branch_pred == "static_not_taken") {
			return make_unique<StaticBranchPredictor>(false);
		} else if (config.branch_pred == "one_bit") {
			return make_unique<OneBitPredictor>();
		} else if (config.branch_pred == "two_bit") {
			return make_unique<TwoBitPredictor>();
		} else {
			throw invalid_argument("Invalid branch prediction strategy");
		}
	}()),
	alus(config.alu_count, config.rs_size, [] {
		return make_unique<ArithmeticLogicUnit>();
	}),
	muls(config.mul_count, config.rs_size, [] {
		return make_unique<MulDivUnit>();
	}),
	ctrls(config.ctrl_count, config.rs_size, [this] {
		return make_unique<ControlUnit>(end, WORD_BYTES);
	}),
	vecs(config.vec_count, config.rs_size, [] {
		return make_unique<VectorUnit>();
	}),
	lsus(config.lsu_count, config.lsq_size, mem),
	exec_paths {&alus, &muls, &ctrls, &lsus} {
	regs.write(2, MEM_SIZE - WORD_BYTES); // stack pointer
	regs.write(1, end); // return address
}

void CPU::step() {
	log.clear();
	cycle_count++;
	jumped = false;
	stalled = false;

	commit();

	writeback();

	if (jumped || halted) {
		if (on_step_callback) on_step_callback(jumped, stalled, log, readout());
		return;
	}

	issue();
	execute();

	decode();
	dispatch();

	fetch();

	if (on_step_callback) on_step_callback(jumped, stalled, log, readout());
}

ExecPath& CPU::get_path(Op op) {
	switch (exec_type(op)) {
		case LOGIC:
			return alus;
		case MULDIV:
			return muls;
		case CTRL:
			return ctrls;
		case VECTOR:
			return vecs;
		case LOADSTORE:
			return lsus;
	}

	throw invalid_argument("Invalid operation");
}

void CPU::read_operand(uint8_t rs, uint32_t &V, uint32_t &Q) {
	if (rs == 0) {
		V = 0;
		Q = -1U;
		return;
	}

	uint32_t src_tag = rat.get(rs);
	if (src_tag == -1U) {
		V = regs.read(rs);
		Q = -1U;
		return;
	}

	auto &src = rob.get(src_tag);
	if (src.ready) {
		V = src.value;
		Q = -1U;
	} else {
		V = 0;
		Q = src_tag;
	}
}

void CPU::flush(uint32_t tag) {
	fetch_q.clear();
	decode_q.clear();
	for (const auto &path : exec_paths)
		path->flush(tag);
	rob.flush(tag);
	rat.rebuild(rob);
}

string CPU::readout() {
	string s = out.str();
	out.str("");
	out.clear();
	return s;
}

void CPU::fetch() {
	while (fetch_q.size() < width) {
		try {
			fetch_q.push_back({pc, mem.loadw(pc)});
		} catch (const out_of_range &) {
			halted = true;
			out << "Instruction fetch out of range. Halting CPU. ";
			return;
		}
		pc += WORD_BYTES;
	}
}

void CPU::decode() {
	size_t decoded = 0;
	while (decoded++ < width && !fetch_q.empty()) {
		auto &fetch = fetch_q.front();
		Instruction instr = Decoder::decode(fetch.instr);

		bool pred_taken = false;
		uint32_t pred_target = fetch.pc + WORD_BYTES;

		if (is_branch(instr.op) || is_jump(instr.op)) {
			pred_taken = branch_pred->predict(fetch.pc, instr);
			if (pred_taken) pred_target = fetch.pc + instr.imm;
		}

		decode_q.push_back({fetch.pc, instr, pred_taken, pred_target});
		fetch_q.pop_front();

		if (pred_taken) {
			fetch_q.clear();
			pc = pred_target;
			break;
		}
	}
}

void CPU::dispatch() {
	size_t dispatched = 0;
	while (dispatched++ < width && !decode_q.empty()) {
		auto &decode = decode_q.front();

		Op op = decode.instr.op;
		uint8_t rd = decode.instr.rd, rs1 = decode.instr.rs1, rs2 = decode.instr.rs2;
		uint32_t pc = decode.pc;
		int32_t imm = decode.instr.imm;

		if (op == INVALID) {
			decode_q.pop_front();
			return;
		}

		ExecPath &path = get_path(op);
		if (!path.can_allocate()) {
			if (is_load(op) || is_store(op))
				out << "Load/store queue full. Stalling pipeline. ";
			else
				out << "Reservation stations full. Stalling pipeline. ";
			stalled = true;
			return;
		}

		uint32_t tag = rob.allocate(decode);
		if (tag == -1U) {
			out << "Re-order buffer full. Stalling pipeline. ";
			stalled = true;
			return;
		}

		uint32_t Qj = -1U, Qk = -1U, Vj = 0, Vk = 0;
		read_operand(rs1, Vj, Qj);
		read_operand(rs2, Vk, Qk);

		path.dispatch({true, op, Vj, Vk, Qj, Qk, pc, imm, tag});

		if (writes_register(op) && rd != 0)
			rat.set(rd, tag);

		decode_q.pop_front();
	}
}

void CPU::issue() {
	for (const auto &path : exec_paths)
		path->issue();
}

void CPU::execute() {
	for (const auto &path : exec_paths)
		path->execute();
}

void CPU::writeback() {
	for (const auto &path : exec_paths) {
		for (const auto &exec : path->take_finished()) {
			rob.update(exec);
			if (writes_register(exec.op))
				for (const auto &other : exec_paths)
					other->wake(exec.tag, exec.value);

			if (is_control(exec.op)) {
				auto &entry = rob.get(exec.tag);

				bool taken = exec.jumped;
				uint32_t target =
					taken ? exec.target : entry.pc + WORD_BYTES;

				if (is_branch(exec.op))
					branch_pred->update(entry.pc, taken);

				bool mispred =
					taken != entry.pred_taken ||
					target != entry.pred_target;

				if (mispred) {
					flush(exec.tag);
					jumped = true;
					pc = target;
					out << "Branch misprediction: flushing pipeline. ";
					branch_mispreds++;
					return;
				}

				branch_preds++;
			}
		}
	}
}

void CPU::commit() {
	size_t committed = 0;
	while (committed++ < width && rob.can_commit()) {
		auto entry = rob.front();

		if (is_load(entry.op) || is_store(entry.op))
			if (!lsus.commit(entry.tag, log)) return;

		if (writes_register(entry.op) && entry.rd != 0) {
			regs.write(entry.rd, entry.value);
			if (rat.get(entry.rd) == entry.tag)
				rat.set(entry.rd, -1U);
		}

		rob.pop();
		instruction_count++;

		if (entry.should_halt) {
			halted = true; 
			out << "Returned outside the program range (0x" << hex << end << dec << "). Halting CPU. ";
			return;
		}
	}
}
