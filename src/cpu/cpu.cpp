#include "cpu.hpp"

CPU::CPU(Program prog) : 
	pc(prog.entry_point), end(prog.exit_point),
	mem(prog.instrs, MEM_SIZE), regs(NUM_REGISTERS, log),
	rob(NUM_REGISTERS * 2), rat(NUM_REGISTERS),
	alus(([&] {
		vector<unique_ptr<ExecUnit>> units; units.reserve(ALU_COUNT);
		for (size_t i = 0; i < ALU_COUNT; i++) units.emplace_back(make_unique<ArithmeticLogicUnit>());
		return units;
	}()), RS_SIZE),
	muls(([&] {
		vector<unique_ptr<ExecUnit>> units; units.reserve(MUL_COUNT);
		for (size_t i = 0; i < MUL_COUNT; i++) units.emplace_back(make_unique<MulDivUnit>());
		return units;
	}()), RS_SIZE),
	brus(([&] {
		vector<unique_ptr<ExecUnit>> units; units.reserve(BRU_COUNT);
		for (size_t i = 0; i < BRU_COUNT; i++) units.emplace_back(make_unique<BranchUnit>(end, WORD_BYTES));
		return units;
	}()), RS_SIZE),
	lsus(([&] {
		vector<unique_ptr<ExecUnit>> units; units.reserve(LSU_COUNT);
		for (size_t i = 0; i < LSU_COUNT; i++) units.emplace_back(make_unique<LoadStoreUnit>(mem, lsq));
		return units;
	}()), RS_SIZE, lsq),
	exec_paths {&alus, &muls, &brus, &lsus} {
	regs.write(2, MEM_SIZE - WORD_BYTES);
}

void CPU::step() {
	log.clear();
	cycle_count++;
	jumped = false;

	commit();
	writeback();

	if (jumped || halted) {
		if (onStepCallback) onStepCallback(jumped, log, readout());
		return;
	}

	issue();
	execute();

	decode();
	dispatch();

	fetch();

	if (onStepCallback) onStepCallback(jumped, log, readout());
}

ExecPath& CPU::pathFor(Op op) {
	if (isALU(op) || isALUI(op) || isUI(op)) return alus;
	if (isMUL(op))                           return muls;
	if (isBranch(op) || isJAL(op))           return brus;
	if (isLoad(op) || isStore(op))           return lsus;
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
		fetch_q.push_back({pc, mem.loadw(pc)});
		pc += WORD_BYTES;
	}
}

void CPU::decode() {
	while (!fetch_q.empty()) {
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
	while (!decode_q.empty()) {
		auto &decode = decode_q.front();

		Op op = decode.instr.op;
		uint8_t rd = decode.instr.rd, rs1 = decode.instr.rs1, rs2 = decode.instr.rs2;
		uint32_t pc = decode.pc;
		int32_t imm = decode.instr.imm;

		if (op == INVALID) {
			decode_q.pop_front();
			return;
		}

		ExecPath &path = pathFor(op);
		RSEntry* rs = path.find_slot();
		if (!rs) {
			out << "Reservation stations full. Stalling pipeline. ";
			return;
		}

		uint32_t tag = rob.allocate(op, rd);
		if (tag == -1U) {
			out << "Re-order buffer full. Stalling pipeline. ";
			return;
		}

		uint32_t Qj = -1U, Qk = -1U, Vj = 0, Vk = 0;
		readOperand(rs1, Vj, Qj);
		readOperand(rs2, Vk, Qk);

		*rs = {true, op, Vj, Vk, Qj, Qk, pc, imm, tag};
		path.allocate(op, tag);

		if (writesRegister(op) && rd != 0)
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
			if (writesRegister(exec.op))
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
	while(rob.canCommit()) {
		auto entry = rob.front();

		if (isLoad(entry.op) || isStore(entry.op)) lsq.commit(entry.tag, mem, log);

		if (writesRegister(entry.op) && entry.rd != 0) {
			regs.write(entry.rd, entry.value);
			if (rat.get(entry.rd) == entry.tag)
				rat.set(entry.rd, -1U);
		}

		rob.pop();
		instruction_count++;

		if (entry.should_halt) {
			halted = true; 
			out << "Reached end of program. Halting CPU. ";
			return;
		}
	}
}
