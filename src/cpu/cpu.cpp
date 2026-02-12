#include "cpu.hpp"

CPU::CPU(Program prog, bool isPipelined, bool isForwarding) :
	mem(prog.instrs, MEM_SIZE), regs(log), lsu(mem, log),
	pipelined(isPipelined), pipe(isForwarding), pc(prog.entryPoint), end(prog.exitPoint) {
	regs.write(2, MEM_SIZE - WORD_BYTES);
}

void CPU::step() {
	if (!pipelined) { stepSequential(); return; }

	log.clear();
	cycleCount++;
	
	PipelineControl ctrl = pipe.getControl();
	if (pipe.memwb.valid) instructionCount++;

	writeback();
	memory();

	// control hazard
	if (ctrl.jumped) {
		pc = ctrl.target;
		pipe.flush();
		if (!ctrl.should_halt) out << "Control hazard: flushing pipeline, jumping to 0x" << hex << setw(8) << setfill('0') << pc << dec << ". ";
		if (onStepCallback) onStepCallback(ctrl, log, readout());
		return;
	}

	execute();

	// data hazard
	if (!ctrl.stall) {
		decode();
		if (fetch()) pc += WORD_BYTES;
	} else {
		out << "Data hazard: stalling pipeline. ";
	}

	if (onStepCallback) onStepCallback(ctrl, log, readout());
}

void CPU::stepSequential() {
	log.clear();

	fetch();
	pc += WORD_BYTES;

	decode();
	execute();

	PipelineControl ctrl = pipe.getControl();
	if (ctrl.jumped) pc = ctrl.target;

	memory();
	writeback();

	instructionCount++;
	cycleCount += 5;

	if (onStepCallback) onStepCallback(ctrl, log, readout());
}

string CPU::readout() {
	string s = out.str();
	out.str("");
	out.clear();
	return s;
}

bool CPU::fetch() {
	auto &ifid = pipe.ifid;
	if (ifid.valid) return false;
	ifid = {pc, mem.loadw(pc), true};
	return true;
}

void CPU::decode() {
	auto &ifid = pipe.ifid; auto &idex = pipe.idex;
	if (!ifid.valid) return;

	Instruction instr = Decoder::decode(ifid.instr);

	uint32_t r1 = regs.read(instr.rs1);
	uint32_t r2 = regs.read(instr.rs2);
	
	r1 = pipe.applyForwarding(instr.rs1, r1);
	r2 = pipe.applyForwarding(instr.rs2, r2);

	idex = {ifid.pc, instr, r1, r2, true};
	ifid.valid = false;
}

void CPU::execute() {
	auto &idex = pipe.idex; auto &exmem = pipe.exmem;
	if (!idex.valid) return;

	Op op = idex.instr.op;
	uint32_t imm = idex.instr.imm, r1 = idex.r1, r2 = idex.r2;
	
	bool jumped = false;
	bool should_halt = false;
	uint32_t alu_out = 0;

	if      (op == LUI)   alu_out = alu.exec(op, 0U, imm);
	else if (op == AUIPC) alu_out = alu.exec(op, idex.pc, imm);

	else if (isMUL(op))                               alu_out = mul.exec(op, r1, r2);
	else if (isALU(op))                               alu_out = alu.exec(op, r1, r2);
	else if (isALUI(op) || isLoad(op) || isStore(op)) alu_out = alu.exec(op, r1, imm);
	
	else if (isBranch(op)) {
		jumped = bru.exec(op, r1, r2);
		r2 = alu.exec(ADD, idex.pc, imm);  // target in r2
	}
	
	else if (isJAL(op)) {
		jumped = true;
		alu_out = alu.exec(ADD, idex.pc, WORD_BYTES);     // return address in alu
		r2 = (op == JAL) ?                                // target in r2
			alu.exec(ADD, idex.pc, imm) :
			alu.exec(ANDI, alu.exec(ADD, r1, imm), ~1U);  // ensure aligned

		if (r2 >= end) {
			should_halt = true;
			out << "Reached end of program at 0x" << hex << setw(8) << setfill('0') << r2 << dec << ". Halting CPU. ";
		}
	}

	exmem = {idex.pc, idex.instr, alu_out, r2, jumped, should_halt, true};
	idex.valid = false;
}

void CPU::memory() {
	auto &exmem = pipe.exmem; auto &memwb = pipe.memwb;
	if (!exmem.valid) return;

	Op op = exmem.instr.op;
	uint32_t addr = exmem.alu;
	uint32_t val = exmem.r2;
	if (isLoad(op)) val = lsu.load(op, addr);
	else if (isStore(op)) lsu.store(op, addr, val);

	memwb = {exmem.pc, exmem.instr, addr, val, exmem.should_halt, true};
	exmem.valid = false;
}

void CPU::writeback() {
	auto &memwb = pipe.memwb;
	if (!memwb.valid) return;

	Op op = memwb.instr.op;

	if (writesRegister(op)) {
		uint8_t rd = memwb.instr.rd;
		if (isLoad(op)) regs.write(rd, memwb.mem);
		else            regs.write(rd, memwb.alu);
	}

	if (memwb.should_halt) halted = true;
	memwb.valid = false;
}
