#include "cpu.hpp"

CPU::CPU(const Config &config) : 
	config_name(config.name),
	mem(MEM_SIZE), regs(NUM_REGISTERS, log), fregs(NUM_FLOAT_REGISTERS, flog),
	vregs(NUM_VECTOR_REGISTERS, vlog),
	width(config.pipe_width), rob(NUM_REGISTERS * 2), rat(NUM_REGISTERS), frat(NUM_FLOAT_REGISTERS),
	vrat(NUM_VECTOR_REGISTERS), vec_config(config.vector_bits),
	branch_pred([config]() -> unique_ptr<BranchPredictor> {
		if (config.branch_pred == "static_taken") {
			return make_unique<StaticBranchPredictor>(true);
		} else if (config.branch_pred == "static_not_taken") {
			return make_unique<StaticBranchPredictor>(false);
		} else if (config.branch_pred == "btfnt") {
			return make_unique<BTFNT>();
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
	ctrls(config.ctrl_count, config.rs_size, [] {
		return make_unique<ControlUnit>();
	}),
	fpus(config.fpu_count, config.rs_size, [] {
		return make_unique<FloatingPointUnit>();
	}),
	vecs(config.vec_count, config.rs_size, [this] {
		return make_unique<VectorUnit>(vec_config);
	}),
	lsus(config.lsu_count, config.rs_size, config.lsq_size, mem),
	exec_paths {&alus, &muls, &ctrls, &lsus, &fpus, &vecs} {
	if (config.vector_bits != 128 && config.vector_bits != 256 && config.vector_bits != 512)
		throw invalid_argument("Invalid vector register width");
}

void CPU::reset() {
	fetch_stopped = false;
	stalled = false;
	halted_ = false;
	jumped = false;
	pc = entry;

	stats = Stats();
	out.str("");
	out.clear();

	branch_pred->clear();
	vec_config.clear();

	fetch_q.clear();
	decode_q.clear();

	rob.clear();
	rat.clear();
	frat.clear();
	vrat.clear();
	for(const auto &path : exec_paths)
		path->clear();

	regs.clear();
	fregs.clear();
	vregs.clear();
	log.clear();
	flog.clear();
	vlog.clear();

	// mem.clear();
	regs.write(2, Value::scalar(MEM_SIZE - WORD_BYTES)); // stack pointer
	regs.write(1, Value::scalar(end)); // return address
}

void CPU::load(const Program &program) {
	program_name = program.name;
	entry = program.entry_point;
	end = program.end_point;
	mem = Memory(program.instrs, MEM_SIZE);
	reset();
}

string CPU::readout() {
	string s = out.str();
	out.str("");
	out.clear();
	return s;
}

Snapshot CPU::snapshot() const {
	RegisterState r;
	PipelineState p;

	r.vec_state = vec_config.snapshot();
	r.rat = rat.snapshot();
	r.frat = frat.snapshot();
	r.vrat = vrat.snapshot();

	for (const auto &reg : regs.snapshot())
		r.regs.push_back(reg.as_scalar());
	for (const auto &reg : fregs.snapshot())
		r.fregs.push_back(reg.as_scalar());
	for (const auto &reg : vregs.snapshot())
		r.vregs.push_back(VectorRegister(reg.lanes));

	p.rob = rob.snapshot();
	p.alus = alus.snapshot();
	p.muls = muls.snapshot();
	p.ctrls = ctrls.snapshot();
	p.fpus = fpus.snapshot();
	p.vecs = vecs.snapshot();
	p.lsus = lsus.snapshot();
	p.fetch_q.assign(fetch_q.begin(), fetch_q.end());
	p.decode_q.assign(decode_q.begin(), decode_q.end());

	Snapshot s;
	s.program_name = program_name;
	s.config_name = config_name;

	s.halted = halted_;
	s.stalled = stalled;
	s.jumped = jumped;
	s.pc = pc;

	s.stats = stats;

	s.registers = r;
	s.pipeline = p;

	return s;
}

void CPU::step() {
	if (halted_) return;

	out << "\n";
	log.clear(); flog.clear(); vlog.clear();
	stats.cycle_count++;
	jumped = false;
	stalled = false;

	commit();
	writeback();

	if (!jumped) {
		execute();
		issue();
		dispatch();
		if (!(stalled || halted_)) {
			decode();
			fetch();
		}
	}

	check_halt();
}

ExecPath& CPU::get_path(Op op) {
	switch (exec_type(op)) {
		case ExecType::LOGIC: return alus;
		case ExecType::MULDIV: return muls;
		case ExecType::CTRL: return ctrls;
		case ExecType::FLOAT: return fpus;
		case ExecType::VECTOR: return vecs;
		case ExecType::LOADSTORE: return lsus;
	}

	throw invalid_argument("Invalid operation");
}

RegisterFile& CPU::regfile(RegType type) {
	switch (type) {
		case RegType::INT: return regs;
		case RegType::FLOAT: return fregs;
		case RegType::VECTOR: return vregs;
		default: throw invalid_argument("Invalid register file");
	}
}

RegisterAliasTable& CPU::alias_table(RegType type) {
	switch(type) {
		case RegType::INT: return rat;
		case RegType::FLOAT: return frat;
		case RegType::VECTOR: return vrat;
		default: throw invalid_argument("Invalid alias table");
	}
}

void CPU::read_operand(uint8_t rs, RegType type, Value &V, uint32_t &Q) {
	if (type == RegType::NONE || (type == RegType::INT && rs == 0)) {
		V = Value::scalar(0);
		Q = -1U;
		return;
	}

	RegisterAliasTable &rat = alias_table(type);
	RegisterFile &regs = regfile(type);

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
		V = Value::scalar(0);
		Q = src_tag;
	}
}

void CPU::flush(uint32_t tag) {
	fetch_q.clear();
	decode_q.clear();
	for (const auto &path : exec_paths)
		path->flush(tag);
	rob.flush(tag);
	rat.rebuild(rob, RegType::INT);
	frat.rebuild(rob, RegType::FLOAT);
	vrat.rebuild(rob, RegType::VECTOR);
	if (vec_config.tag != -1U && vec_config.tag > tag) vec_config.tag = -1U;
}

void CPU::fetch() {
	while (!fetch_stopped && fetch_q.size() < width) {
		if (pc > (end - WORD_BYTES)) {
			fetch_stopped = true;
			out << "Reached end of program. Draining pipeline. ";
			return;
		}

		try {
			fetch_q.push_back({pc, mem.loadw(pc)});
		} catch (const out_of_range &) {
			fetch_stopped = true;
			out << "Instruction fetch out of range. Draining pipeline. ";
			return;
		}
		pc += WORD_BYTES;
	}
}

void CPU::decode() {
	while (!fetch_q.empty()) {
		auto &fetch = fetch_q.front();
		Instruction instr = Decoder::decode(fetch.instr);

		bool pred_taken = false;
		uint32_t pred_target = fetch.pc + WORD_BYTES;

		if (is_ctrl(instr.op)) {
			auto prediction = branch_pred->predict(fetch.pc, instr);
			pred_taken = prediction.taken;
			pred_target = prediction.target;
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
	while (!decode_q.empty()) {
		auto &decode = decode_q.front();

		Op op = decode.instr.op;
		uint8_t rd = decode.instr.rd, rs1 = decode.instr.rs1, rs2 = decode.instr.rs2, rs3 = decode.instr.rs3, rm = decode.instr.rm;
		uint32_t pc = decode.pc;
		int32_t imm = decode.instr.imm;

		if (op == INVALID) {
			halted_ = true;
			out << "Invalid instruction at 0x" << hex << pc << dec << ". Halting CPU. ";
			return;
		}

		ExecPath &path = get_path(op);
		if (!path.can_allocate()) {
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

		uint32_t Qj = -1U, Qk = -1U, Ql = -1U, Qv = -1U;
		Value Vj = 0, Vk = 0, Vl = 0;
		read_operand(rs1, src_type(op, 0), Vj, Qj);
		read_operand(rs2, src_type(op, 1), Vk, Qk);
		read_operand(rs3, src_type(op, 2), Vl, Ql);

		uint8_t vl = vec_config.vl;
		uint8_t sew = vec_config.vsew_bits;
		if (is_vset(op)) sew = decode.instr.sew;
		if (is_vec(op) && !is_vset(op) && vec_config.tag != -1U) {
			auto &vset = rob.get(vec_config.tag);
			if (vset.ready) {
				vl = vset.vl;
				sew = vset.sew;
			} else {
				Qv = vec_config.tag;
			}
		}

		path.dispatch({true, op, Vj, Vk, Vl, Qj, Qk, Ql, Qv, pc, imm, rm, vl, sew, tag});

		if (is_vset(op)) vec_config.tag = tag;

		RegType dst = dst_type(op);
		if (dst != RegType::NONE && !(dst == RegType::INT && rd == 0))
			alias_table(dst).set(rd, tag);

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
					other->wake(exec);

		}
	}

	for (auto &entry : rob.get_entries()) {
		if (!is_ctrl(entry.op)) continue;
		if (entry.ctrl_handled) continue;
		if (!entry.ready) return;

		entry.ctrl_handled = true;
		if (is_branch(entry.op)) stats.branch_count++;

		bool taken = entry.jumped;
		uint32_t target = taken ? entry.target : entry.pc + WORD_BYTES;

		branch_pred->update(entry.pc, entry.op, taken, target);

		bool mispred =
			taken != entry.pred_taken ||
			target != entry.pred_target;

		if (mispred) {
			if (is_branch(entry.op)) stats.mispred_count++;
			flush(entry.tag);
			jumped = true;
			fetch_stopped = false;
			pc = target;
			out << "Branch misprediction: flushing pipeline. ";
			return;
		}
	}
}

void CPU::check_halt() {
	if (fetch_stopped && fetch_q.empty() && decode_q.empty() && rob.empty()) {
		halted_ = true;
		out << "Pipeline drained. Halting CPU. ";
	}
}

void CPU::commit() {
	size_t committed = 0;
	while (committed++ < width && rob.can_commit()) {
		auto entry = rob.front();

		if (is_load(entry.op) || is_store(entry.op))
			if (!lsus.commit(entry.tag, log)) return;

		RegType dst = dst_type(entry.op);
		if (dst != RegType::NONE && !(dst == RegType::INT && entry.rd == 0)) {
			regfile(dst).write(entry.rd, entry.value);
			if (alias_table(dst).get(entry.rd) == entry.tag)
				alias_table(dst).set(entry.rd, -1U);
		}

		if (is_vset(entry.op)) {
			vec_config.apply(entry.vl, entry.sew);
			if (vec_config.tag == entry.tag)
				vec_config.tag = -1U;
		}

		rob.pop();
		stats.instruction_count++;
	}
}
