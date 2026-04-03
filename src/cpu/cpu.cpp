#include "cpu.hpp"

CPU::CPU(Program prog) : 
	pc(prog.entry_point), end(prog.entry_point + prog.instrs.size()),
	mem(prog.instrs, MEM_SIZE), regs(NUM_REGISTERS, log),
	rob(NUM_REGISTERS * 2), rat(NUM_REGISTERS),
	alus(ALU_COUNT, RS_SIZE, [] {
		return make_unique<ArithmeticLogicUnit>();
	}),
	muls(MUL_COUNT, RS_SIZE, [] {
		return make_unique<MulDivUnit>();
	}),
	ctrls(CTRL_COUNT, RS_SIZE, [this] {
		return make_unique<ControlUnit>(end, WORD_BYTES);
	}),
	lsus(LSU_COUNT, RS_SIZE, lsq, [this] {
		return make_unique<LoadStoreUnit>(mem, lsq);
	}),
	exec_paths {&alus, &muls, &ctrls, &lsus} {
	regs.write(2, MEM_SIZE - WORD_BYTES); // stack pointer
	regs.write(1, end); // return address
}

void CPU::step() {
	log.clear();
	cycle_count++;
	jumped = false;

	commit();
	writeback();

	if (jumped || halted) {
		if (on_step_callback) on_step_callback(jumped, log, readout());
		return;
	}

	issue();
	execute();

	decode();
	dispatch();

	fetch();

	if (on_step_callback) on_step_callback(jumped, log, readout());
}

ExecPath& CPU::get_path(Op op) {
	switch (exec_type(op)) {
		case LOGIC:
			return alus;
		case MULDIV:
			return muls;
		case CTRL:
			return ctrls;
		case LOADSTORE:
			return lsus;
	}

	throw invalid_argument("Invalid operation");
}

void CPU::flush(uint32_t tag) {
	fetch_q.clear();
	decode_q.clear();
	for (auto *path : exec_paths)
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
	while (fetch_q.size() < PIPELINE_WIDTH) {
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
	while (decoded++ < PIPELINE_WIDTH && !fetch_q.empty()) {
		auto &fetch = fetch_q.front();
		Instruction instr = Decoder::decode(fetch.instr);
		decode_q.push_back({fetch.pc, instr});
		fetch_q.pop_front();
	}
}

void CPU::readOperand(uint8_t rs, uint32_t &V, uint32_t &Q) {
	if (rs == 0) {
		V = 0;
		Q = -1U;
		return;
	}

	uint32_t prod_tag = rat.get(rs);
	if (prod_tag == -1U) {
		V = regs.read(rs);
		Q = -1U;
		return;
	}

	auto &prod = rob.get(prod_tag);
	if (prod.ready) {
		V = prod.value;
		Q = -1U;
	} else {
		V = 0;
		Q = prod_tag;
	}
}

void CPU::dispatch() {
	size_t dispatched = 0;
	while (dispatched++ < PIPELINE_WIDTH && !decode_q.empty()) {
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
		RSEntry* rs = path.find_slot();
		if (!rs) {
			out << "Reservation stations full. Stalling pipeline. ";
			return;
		}

		uint32_t tag = rob.allocate(op, rd, pc);
		if (tag == -1U) {
			out << "Re-order buffer full. Stalling pipeline. ";
			return;
		}

		uint32_t Qj = -1U, Qk = -1U, Vj = 0, Vk = 0;
		readOperand(rs1, Vj, Qj);
		readOperand(rs2, Vk, Qk);

		*rs = {true, op, Vj, Vk, Qj, Qk, pc, imm, tag};
		path.allocate(op, tag);

		if (writes_register(op) && rd != 0)
			rat.set(rd, tag);

		decode_q.pop_front();
	}
}

void CPU::issue() {
	for (auto *path : exec_paths)
		path->issue();
}

void CPU::execute() {
	for (auto *path : exec_paths)
		path->execute();
}

void CPU::writeback() {
	for (auto *path : exec_paths) {
		for (const auto &exec : path->take_finished()) {
			rob.set(exec.tag, exec.value, exec.addr, exec.jumped, exec.should_halt);
			if (writes_register(exec.op))
				for (auto *other : exec_paths)
					other->wake(exec.tag, exec.value);

			if (exec.jumped) {
				flush(exec.tag);
				jumped = true;
				pc = exec.target;
				out << "Control hazard: flushing pipeline, jumping to 0x" << hex << exec.target << dec << ". ";
				return;
			}
		}
	}
}

void CPU::commit() {
	size_t committed = 0;
	while (committed++ < PIPELINE_WIDTH && rob.can_commit()) {
		auto entry = rob.front();

		if (is_load(entry.op) || is_store(entry.op)) lsq.commit(entry.tag, mem, log);

		if (writes_register(entry.op) && entry.rd != 0) {
			regs.write(entry.rd, entry.value);
			if (rat.get(entry.rd) == entry.tag)
				rat.set(entry.rd, -1U);
		}

		rob.pop();
		instruction_count++;

		if (entry.should_halt) {
			halted = true; 
			out << "Returned outside the program range (0x" << hex << end << "). Halting CPU. ";
			return;
		}
	}
}
