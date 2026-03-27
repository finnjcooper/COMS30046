#include "cpu.hpp"

CPU::CPU(Program prog) :
	mem(prog.instrs, MEM_SIZE), regs(NUM_REGISTERS, log), rat(NUM_REGISTERS), rob(NUM_REGISTERS * 2), pc(prog.entry_point), end(prog.exit_point) {
	regs.write(2, MEM_SIZE - WORD_BYTES);
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
	execute();
	decode();
	issue();
	fetch();

	if (onStepCallback) onStepCallback(jumped, log, readout());
}

void CPU::flush() {
	fetch_q.clear();
	decode_q.clear();
	rs_alu.clear();
	rs_mul.clear();
	rs_bru.clear();
	rs_lsu.clear();
	for (auto *alu : alus) alu->flush();
	for (auto *mul : muls) mul->flush();
	for (auto *bru : brus) bru->flush();
	for (auto *lsu : lsus) lsu->flush();
	rob.flush();
	rat.flush();
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

void CPU::issue() {
	while (!decode_q.empty()) {
		auto &decode = decode_q.front();

		Op op = decode.instr.op;
		uint8_t rd = decode.instr.rd, rs1 = decode.instr.rs1, rs2 = decode.instr.rs2;
		uint32_t pc = decode.pc;
		int32_t imm = decode.instr.imm;

		uint32_t tag = rob.allocate(op, rd);
		if (tag == -1U) {
			out << "ROB full. Stalling pipeline. ";
			return;
		}

		uint32_t Qj = 0, Vj = 0, Qk = 0, Vk = 0;

		if (rat.get(rs1) != -1U) Qj = rat.get(rs1);
		else                     Vj = regs.read(rs1);
		
		if (rat.get(rs2) != -1U) Qk = rat.get(rs2);
		else                     Vk = regs.read(rs2);

		RSEntry entry = {true, op, Vj, Vk, Qj, Qk, pc, imm, tag};

		if (isALU(op) || isALUI(op) || isUI(op)) rs_alu.push_back(entry);
		else if (isMUL(op))                      rs_mul.push_back(entry);
		else if (isBranch(op) || isJAL(op))      rs_bru.push_back(entry);
		else if (isLoad(op) || isStore(op))      rs_lsu.push_back(entry);
		
		if (writesRegister(op) && rd != 0) rat.set(rd, tag);
		decode_q.pop_front();
	}
}

void tryIssue(vector<RSEntry> &rs_vec, vector<ExecUnit*> &units) {
	for (auto &entry : rs_vec) {
		if (!entry.busy) continue;
		if (entry.Qj == 0 && entry.Qk == 0) {
			for (auto *unit : units) {
				if (!unit->busy()) {
					unit->start(entry);
					entry.busy = false; // need to remove from rs
					break;
				}
			}
		}
	}
}

void CPU::execute() {
	tryIssue(rs_alu, alus);
	tryIssue(rs_mul, muls);
	tryIssue(rs_bru, brus);
	tryIssue(rs_lsu, lsus);

	for (auto *alu : alus) alu->step();
	for (auto *mul : muls) mul->step();
	for (auto *bru : brus) bru->step();
	for (auto *lsu : lsus) lsu->step();
}

void CPU::writeback() {
	auto handleResult = [&](ExecEntry exec) {
		uint32_t tag = exec.tag;
		rob.set(tag, exec.value, exec.addr);

		auto broadcast = [&](RSEntry &rs) {
			if (rs.Qj == tag) {
				rs.Vj = exec.value;
				rs.Qj = 0;
			}
			if (rs.Qk == tag) {
				rs.Vk = exec.value;
				rs.Qk = 0;
			}
		};

		for (auto &rs : rs_alu) broadcast(rs);
		for (auto &rs : rs_mul) broadcast(rs);
		for (auto &rs : rs_bru) broadcast(rs);
		for (auto &rs : rs_lsu) broadcast(rs);

		if (exec.jumped) {
			pc = exec.target;
			if (exec.should_halt) { halted = true; out << "Reached end of program. Halting CPU. "; }
			else out << "Control hazard: flushing pipeline, jumping to 0x" << hex << setw(8) << setfill('0') << pc << dec << ". ";

			flush();

			jumped = true;
		}
	};

	for (auto *alu : alus) if (alu->done()) handleResult(alu->getResult());
	for (auto *mul : muls) if (mul->done()) handleResult(mul->getResult());
	for (auto *bru : brus) if (bru->done()) handleResult(bru->getResult());
	for (auto *lsu : lsus) if (lsu->done()) handleResult(lsu->getResult());
}

void CPU::commit() {
	while(rob.canCommit()) {
		auto &entry = rob.front();

		if (isStore(entry.op)) mem.storew(entry.addr, entry.value);

		if (writesRegister(entry.op) && entry.dest_reg != 0) {
			regs.write(entry.dest_reg, entry.value);
			if (rat.get(entry.dest_reg) == entry.tag)
				rat.set(entry.dest_reg, -1U);
		}

		rob.pop();

		instruction_count++;
	}
}
