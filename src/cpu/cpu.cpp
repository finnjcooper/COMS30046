#include "cpu.hpp"

CPU::CPU(Program prog, bool pipelined, bool forwarding) :
	mem(prog.instrs, MEM_SIZE), regs(NUM_REGISTERS, log), pc(prog.entry_point), end(prog.exit_point),
	pipelined(pipelined), pipe(forwarding), rob(NUM_REGISTERS * 2) {
	regs.write(2, MEM_SIZE - WORD_BYTES);
}

void CPU::step() {
	if (!pipelined) { stepSequential(); return; }

	log.clear();
	cycle_count++;
	jumped = false;

	writeback();
	execute();
	decode();
	issue();
	fetch();

	if (onStepCallback) onStepCallback(jumped, log, readout());
}

void CPU::stepSequential() {
	log.clear();
	jumped = false;

	fetch();
	decode();
	issue();
	execute();
	writeback();

	instruction_count++;
	cycle_count += 5;

	if (onStepCallback) onStepCallback(jumped, log, readout());
}

string CPU::readout() {
	string s = out.str();
	out.str("");
	out.clear();
	return s;
}

void CPU::fetch() {
	while (pipe.decode_q.size() < PIPELINE_WIDTH) {
		pipe.decode_q.push_back({seq++, pc, mem.loadw(pc)});
		pc += WORD_BYTES;
	}
}

void CPU::decode() {
	vector<Instruction> prevs;
	while (!pipe.decode_q.empty()) {
		auto &decode = pipe.decode_q.front();

		Instruction instr = Decoder::decode(decode.instr);

		if (pipe.hasHazard(prevs, instr)) {
			out << "Data hazard. Stalling pipeline. ";
			return;
		}

		uint32_t tag = -1;
		if (writesRegister(instr.op) && instr.rd != 0) {
			tag = rob.allocate(instr.op, instr.rd);
			if (tag == -1) {
				out << "ROB full. Stalling pipeline. ";
				return;
			}
		}

		prevs.push_back(instr);

		uint32_t r1 = regs.read(instr.rs1);
		uint32_t r2 = regs.read(instr.rs2);
		
		r1 = pipe.applyForwarding(instr.rs1, r1);
		r2 = pipe.applyForwarding(instr.rs2, r2);

		pipe.exec_q.push_back({decode.seq, decode.pc, instr, r1, r2, tag});
		pipe.decode_q.pop_front();
	}
}

void CPU::issue() {
	while (!pipe.exec_q.empty()) {
		auto &exec = pipe.exec_q.front();

		Op op = exec.instr.op;
		bool issued = false;
		if (isMUL(op)) {
			for (auto &mul : muls) {
				if (!mul.busy()) {
					mul.start(exec);
					issued = true;
					break;
				}
			}
		} else if (isALU(op) || isALUI(op) || isUI(op)) {
			for (auto &alu : alus) {
				if (!alu.busy()) {
					alu.start(exec);
					issued = true;
					break;
				}
			}
		} else if (isBranch(op) || isJAL(op)) {
			for (auto &bru : brus) {
				if (!bru.busy()) {
					bru.start(exec);
					issued = true;
					break;
				}
			}
		} else if (isLoad(op) || isStore(op)) {
			for (auto &lsu : lsus) {
				if (!lsu.busy()) {
					lsu.start(exec);
					issued = true;
					break;
				}
			}
		}
		if (!issued) break;
		pipe.exec_q.pop_front();
	}
}

void CPU::execute() {
	for (auto &alu : alus) alu.step();
	for (auto &mul : muls) mul.step();
	for (auto &bru : brus) bru.step();
	for (auto &lsu : lsus) lsu.step();

	for (auto &alu : alus) if (alu.done()) pipe.commit_q.push_back(alu.getResult());
	for (auto &mul : muls) if (mul.done()) pipe.commit_q.push_back(mul.getResult());
	for (auto &bru : brus) if (bru.done()) pipe.commit_q.push_back(bru.getResult());
	for (auto &lsu : lsus) if (lsu.done()) pipe.commit_q.push_back(lsu.getResult());
}

void CPU::writeback() {
	bool not_found = false;
	while (!(not_found || pipe.commit_q.empty())) {
		not_found = true;
		for (auto commit = pipe.commit_q.begin(); commit != pipe.commit_q.end(); commit++) {
			if (commit->seq != next_commit) continue;
			Op op = commit->instr.op;

			if (writesRegister(op) && commit->tag != -1U) {
				if (isLoad(op)) rob.write(commit->tag, commit->alu);
				else rob.write(commit->tag, commit->alu);
			}

			instruction_count++;
			next_commit++;
			not_found = false;

			if (commit->jumped) {
				pc = commit->r2;
				seq = next_commit;
				if (commit->should_halt) { halted = true; out << "Reached end of program. Halting CPU. "; }
				else out << "Control hazard: flushing pipeline, jumping to 0x" << hex << setw(8) << setfill('0') << pc << dec << ". ";
				
				pipe.flush();
				for (auto &alu : alus) alu.flush();
				for (auto &mul : muls) mul.flush();
				for (auto &bru : brus) bru.flush();
				for (auto &lsu : lsus) lsu.flush();

				jumped = true;
				return;
			}

			pipe.commit_q.erase(commit);
			break;
		}
	}
}

void CPU::commit() {
	while(rob.canCommit()) {
		auto &entry = rob.front();

		uint32_t val;
		if (isLoad(entry.op) || isStore(entry.op)) val = LoadStoreUnit(mem, log).exec(entry.op, entry.store_addr, entry.store_value);

		if (writesRegister(entry.op) && entry.dest_reg != 0)
			regs.write(entry.dest_reg, entry.value);
		rob.pop();
	}
}
