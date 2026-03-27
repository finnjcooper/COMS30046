#include "cpu.hpp"

CPU::CPU(Program prog) : 
	mem(prog.instrs, MEM_SIZE), regs(NUM_REGISTERS, log), rat(NUM_REGISTERS), rob(NUM_REGISTERS * 2),
	pc(prog.entry_point), end(prog.exit_point) {
	
	regs.write(2, MEM_SIZE - WORD_BYTES);

	for (size_t i = 0; i < BRU_COUNT; i++) brus.push_back(new BranchUnit(end, WORD_BYTES));
	for (size_t i = 0; i < LSU_COUNT; i++) lsus.push_back(new LoadStoreUnit(mem, lsq));
	for (size_t i = 0; i < ALU_COUNT; i++) alus.push_back(new ArithmeticLogicUnit());
	for (size_t i = 0; i < MUL_COUNT; i++) muls.push_back(new MulDivUnit());
}

CPU::~CPU() {
	for (auto ptr : alus) delete ptr;
	for (auto ptr : muls) delete ptr;
	for (auto ptr : brus) delete ptr;
	for (auto ptr : lsus) delete ptr;
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

void CPU::flush(uint32_t tag) {
	fetch_q.clear();
	decode_q.clear();
	for (auto &rs : rs_alu) if (rs.tag > tag) rs.busy = false;
	for (auto &rs : rs_mul) if (rs.tag > tag) rs.busy = false;
	for (auto &rs : rs_bru) if (rs.tag > tag) rs.busy = false;
	for (auto &rs : rs_lsu) if (rs.tag > tag) rs.busy = false;
	for (auto *alu : alus) alu->flush(tag);
	for (auto *mul : muls) mul->flush(tag);
	for (auto *bru : brus) bru->flush(tag);
	for (auto *lsu : lsus) lsu->flush(tag);
	lsq.flush(tag);
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

void CPU::dispatch() {
	auto readOperand = [&](uint8_t rs, uint32_t &V, uint32_t &Q) {
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
	};

	auto findFreeSlot = [](vector<RSEntry> &rs_vec) -> RSEntry* {
		for (auto &rs : rs_vec)
			if (!rs.busy) return &rs;
		return nullptr;
	};

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

		vector<RSEntry> *target_rs = nullptr;
		if (isALU(op) || isALUI(op) || isUI(op))      target_rs = &rs_alu;
		else if (isMUL(op))                           target_rs = &rs_mul;
		else if (isBranch(op) || isJAL(op))           target_rs = &rs_bru;
		else if (isLoad(op) || isStore(op))           target_rs = &rs_lsu;
		else {
			out << "Unsupported instruction. ";
			return;
		}

		RSEntry *slot = findFreeSlot(*target_rs);
		if (!slot) {
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

		*slot = {true, op, Vj, Vk, Qj, Qk, pc, imm, tag};
		if (isLoad(op) || isStore(op)) lsq.allocate(op, tag);

		if (writesRegister(op) && rd != 0)
			rat.set(rd, tag);

		decode_q.pop_front();
	}
}

void tryIssue(vector<RSEntry> &rs_vec, vector<ExecUnit*> &units) {
	for (auto &rs : rs_vec)
		if (rs.busy && rs.Qj == -1U && rs.Qk == -1U)
			for (auto *unit : units)
				if (!unit->busy()) {
					unit->start(rs);
					rs.busy = false;
					break;
				}
}

RSEntry* findRSEntry(vector<RSEntry> &rs_vec, uint32_t tag) {
	for (auto &rs : rs_vec)
		if (rs.busy && rs.tag == tag) return &rs;
	return nullptr;
}

void tryIssueMemory(vector<RSEntry> &rs_vec, vector<ExecUnit*> &units, LoadStoreQueue &lsq) {
	for (auto *unit : units) {
		if (unit->busy()) continue;

		bool issued = false;
		for (auto &entry : lsq.getEntries()) {
			if (entry.issued || entry.done) continue;

			RSEntry *rs = findRSEntry(rs_vec, entry.tag);
			if (!rs || rs->Qj != -1U || rs->Qk != -1U) continue;

			uint32_t addr = rs->Vj + rs->imm;
			if (isLoad(entry.op) && !lsq.canIssueLoad(entry.tag, addr)) continue;

			unit->start(*rs);
			rs->busy = false;
			lsq.markIssued(entry.tag);
			issued = true;
			break;
		}

		if (!issued) break;
	}
}

void CPU::issue() {
	tryIssue(rs_alu, alus);
	tryIssue(rs_mul, muls);
	tryIssue(rs_bru, brus);
	tryIssueMemory(rs_lsu, lsus, lsq);
}

void CPU::execute() {
	for (auto *alu : alus) alu->step();
	for (auto *mul : muls) mul->step();
	for (auto *bru : brus) bru->step();
	for (auto *lsu : lsus) lsu->step();
}

void CPU::writeback() {
	auto handleResult = [&](ExecEntry exec) {
		uint32_t tag = exec.tag;
		rob.set(tag, exec.value, exec.addr, exec.jumped, exec.should_halt);

		auto broadcast = [&](RSEntry &rs) {
			if (rs.Qj == tag) {
				rs.Vj = exec.value;
				rs.Qj = -1U;
			}
			if (rs.Qk == tag) {
				rs.Vk = exec.value;
				rs.Qk = -1U;
			}
		};

		for (auto &rs : rs_alu) broadcast(rs);
		for (auto &rs : rs_mul) broadcast(rs);
		for (auto &rs : rs_bru) broadcast(rs);
		for (auto &rs : rs_lsu) broadcast(rs);

		if (exec.jumped) {
			flush(exec.tag);
			jumped = true;
			pc = exec.target;
			out << "Control hazard: flushing pipeline, jumping to 0x" << hex << exec.target << dec << ". ";
		}
	};

	for (auto *alu : alus) if (alu->done()) handleResult(alu->getResult());
	for (auto *mul : muls) if (mul->done()) handleResult(mul->getResult());
	for (auto *bru : brus) if (bru->done()) handleResult(bru->getResult());
	for (auto *lsu : lsus) if (lsu->done()) handleResult(lsu->getResult());
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
