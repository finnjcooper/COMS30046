#pragma once
#include <sstream>
#include "regfile.h"
#include "memory.h"
#include "alu.h"
#include "branch.h"
#include "loadstore.h"
#include "instruction.h"
#include "decoder.h"

class CPU {
public:
	~CPU() = default;
	CPU(Program prog) : mem(Memory(prog.instrs, MEM_SIZE)), pc(prog.entryPoint) { regs.write(2, MEM_SIZE - WORD_BYTES); }

	static constexpr size_t MEM_SIZE = 64 * 1024; // 64 KB
	static constexpr uint8_t XLEN = 32;
	static constexpr uint8_t WORD_BYTES = XLEN / 8;

	void step() {
		if (!pipelined) {
			fetch();
			decode();
			execute();
			if (exmem.jump) pc = exmem.alu;
			memory();
			writeback();
			return;
		}

		writeback();
		memory();
		execute();

		// TODO: move to hazard unit
		if (exmem.jump) {
			out << "JUMP detected: target=0x" << hex << exmem.alu << " from PC=0x" << exmem.pc << dec << "\n";
			pc = exmem.alu;
			ifid.valid = false;
			idex.valid = false;
		}
		
		decode();
		fetch();

		cout << "---- Pipeline State ----\n";
		cout << "IF/ID: PC=0x" << hex << ifid.pc << " Instr=0x" << ifid.instr << dec << "\n";
		cout << "ID/EX: PC=0x" << hex << idex.pc << " Op=0x" << (int)idex.instr.op << " rs1=0x" << idex.instr.rs1 << " rs2=0x" << idex.instr.rs2 << " imm=0x" << idex.instr.imm << " D1=0x" << idex.r1 << " D2=0x" << idex.r2 << dec << "\n";
		cout << "EX/MEM: PC=0x" << hex << exmem.pc << " Op=0x" << (int)exmem.instr.op << " rs1=0x" << exmem.instr.rs1 << " rs2=0x" << exmem.instr.rs2 << " imm=0x" << exmem.instr.imm << " D1=0x" << exmem.alu << " D2=0x" << exmem.r2 << dec << "\n";
		cout << "MEM/WB: PC=0x" << hex << memwb.pc << " Op=0x" << (int)memwb.instr.op << " rs1=0x" << memwb.instr.rs1 << " rs2=0x" << memwb.instr.rs2 << " imm=0x" << memwb.instr.imm << " D1=0x" << memwb.alu << " D2=0x" << memwb.mem << dec << "\n";
	}

	bool running() { return !halted; }
	uint32_t getPC() const { return pc; }
	RegisterFile getRegisters() const { return regs; }
	Memory getMemory() const { return mem; }
	
	string readout() {
		string s = out.str();
		out.str("");
		out.clear();
		return s;
	}

private:
	RegisterFile regs;
	LoadStoreUnit lsu;
	BranchUnit bru;
	ALU alu;
	Memory mem;

	IFID ifid;
	IDEX idex;
	EXMEM exmem;
	MEMWB memwb;

	ostringstream out;

	uint32_t pc = 0;
	bool halted = false;
	bool pipelined = false;

	void fetch() {
		if (ifid.valid) return;
		
		ifid = {pc, mem.loadw(pc), true};
		pc += WORD_BYTES;
	}

	void decode() {
		if (ifid.valid == false) return;
		
		Instruction instr = Decoder::decode(ifid.instr);

		// TODO: handle hazards (stall / forwarding)

		idex = {ifid.pc, instr, regs.read(instr.rs1), regs.read(instr.rs2), true};
		ifid.valid = false;
	}
	
	void execute() {
		if (idex.valid == false) return;

		Op op = idex.instr.op;
		uint32_t imm = idex.instr.imm, r1 = idex.r1, r2 = idex.r2;
		bool jump = false;
		uint32_t alu_res;

		if (isALU(op)) alu_res = alu.execute(op, r1, r2);
		if (isALUI(op)) alu_res = alu.execute(op, r1, imm);
		if (isLoad(op) || isStore(op)) alu_res = r1 + imm;
		
		if (isBranch(op)) {
			jump = bru.evaluate(op, r1, r2);
			alu_res = idex.pc + imm;
		}
		
		if (isJAL(op)) {
			jump = true;
			alu_res = op == JAL ? idex.pc + imm : (r1 + imm) & ~1u;
			r2 = idex.pc + WORD_BYTES;
		}

		if (op == LUI) alu_res = imm;
		if (op == AUIPC) alu_res = idex.pc + imm;

		exmem = {idex.pc, idex.instr, alu_res, r2, jump, true};
		idex.valid = false;
	}

	void memory() {
		if (exmem.valid == false) return;

		Op op = exmem.instr.op;
		uint32_t addr = exmem.alu, val = exmem.r2;

		if (isLoad(op)) val = lsu.load(op, mem, addr);
		if (isStore(op)) lsu.store(op, mem, addr, val);
		memwb = {exmem.pc, exmem.instr, addr, val, true};
		exmem.valid = false;
	}

	void writeback() {
		if (memwb.valid == false) return;

		Op op = memwb.instr.op;
		uint8_t rd = memwb.instr.rd;

		if (isALU(op) || isALUI(op) || isUI(op)) regs.write(rd, memwb.alu);
		if (isLoad(op) || isJAL(op)) regs.write(rd, memwb.mem);

		if (op == EBREAK) {
			pc -= WORD_BYTES; // undo pc increment to point to ebreak instruction
			halted = true;
		}

		memwb.valid = false;
	}
};
