#include "cpu.hpp"

CPU::CPU(Program prog, bool pipelined, bool forwarding) :
	mem(prog.instrs, MEM_SIZE), regs(log), pc(prog.entry_point), end(prog.exit_point),
	pipelined(pipelined), pipe(forwarding) {
	regs.write(2, MEM_SIZE - WORD_BYTES);
}

void CPU::step() {
	if (!pipelined) { stepSequential(); return; }

	log.clear();
	cycle_count++;

	bool jumped = writeback();
	execute();
	decode();
	issue();
	fetch();

	if (onStepCallback) onStepCallback(jumped, log, readout());
}

void CPU::stepSequential() {
	log.clear();

	fetch();
	decode();
	issue();
	execute();
	bool jumped = writeback();

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
	while (pipe.ifids.size() < PIPELINE_WIDTH) {
		pipe.ifids.push_back({seq++, pc, mem.loadw(pc)});
		pc += WORD_BYTES;
	}
}

void CPU::decode() {
	vector<Instruction> prevs;
	while (!pipe.ifids.empty()) {
		auto &ifid = pipe.ifids.front();

		Instruction instr = Decoder::decode(ifid.instr);

		if (pipe.hasHazard(prevs, instr)) {
			out << "Data hazard. Stalling pipeline. ";
			return;
		}
		
		prevs.push_back(instr);

		uint32_t r1 = regs.read(instr.rs1);
		uint32_t r2 = regs.read(instr.rs2);
		
		r1 = pipe.applyForwarding(instr.rs1, r1);
		r2 = pipe.applyForwarding(instr.rs2, r2);

		pipe.idexs.push_back({ifid.seq, ifid.pc, instr, r1, r2});
		pipe.ifids.pop_front();
	}
}

void CPU::issue() {
	while (!pipe.idexs.empty()) {
		auto &idex = pipe.idexs.front();

		Op op = idex.instr.op;
		bool issued = false;
		if (isMUL(op)) {
			for (auto &mul : muls) {
				if (!mul.busy()) {
					mul.start(idex);
					issued = true;
					break;
				}
			}
		} else if (isALU(op) || isALUI(op) || isUI(op)) {
			for (auto &alu : alus) {
				if (!alu.busy()) {
					alu.start(idex);
					issued = true;
					break;
				}
			}
		} else if (isBranch(op) || isJAL(op)) {
			for (auto &bru : brus) {
				if (!bru.busy()) {
					bru.start(idex);
					issued = true;
					break;
				}
			}
		} else if (isLoad(op) || isStore(op)) {
			for (auto &lsu : lsus) {
				if (!lsu.busy()) {
					lsu.start(idex);
					issued = true;
					break;
				}
			}
		}
		if (!issued) break;
		pipe.idexs.pop_front();
	}
}

void CPU::execute() {
	for (auto &alu : alus) alu.step();
	for (auto &mul : muls) mul.step();
	for (auto &bru : brus) bru.step();
	for (auto &lsu : lsus) lsu.step();

	for (auto &alu : alus) if (alu.done()) pipe.exmems.push_back(alu.getResult());
	for (auto &mul : muls) if (mul.done()) pipe.exmems.push_back(mul.getResult());
	for (auto &bru : brus) if (bru.done()) pipe.exmems.push_back(bru.getResult());
	for (auto &lsu : lsus) if (lsu.done()) pipe.exmems.push_back(lsu.getResult());
}

bool CPU::writeback() {
	bool not_found = false;
	while (!(not_found || pipe.exmems.empty())) {
		not_found = true;
		for (auto exmem = pipe.exmems.begin(); exmem != pipe.exmems.end(); exmem++) {
			if (exmem->seq != next_commit) continue;
			Op op = exmem->instr.op;

			uint32_t val;
			if (isLoad(op) || isStore(op)) val = LoadStoreUnit(mem, log).exec(op, exmem->alu, exmem->r2);

			if (writesRegister(op))
				if (isLoad(op)) regs.write(exmem->instr.rd, val);
				else regs.write(exmem->instr.rd, exmem->alu);

			instruction_count++;
			next_commit++;
			not_found = false;

			if (exmem->jumped) {
				pc = exmem->r2;
				seq = next_commit;
				if (exmem->should_halt) { halted = true; out << "Reached end of program. Halting CPU. "; }
				else out << "Control hazard: flushing pipeline, jumping to 0x" << hex << setw(8) << setfill('0') << pc << dec << ". ";
				
				pipe.flush();
				for (auto &alu : alus) alu.flush();
				for (auto &mul : muls) mul.flush();
				for (auto &bru : brus) bru.flush();
				for (auto &lsu : lsus) lsu.flush();
				return true;
			}

			pipe.exmems.erase(exmem);
			break;
		}
	}

	return false;
}
