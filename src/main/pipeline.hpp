#pragma once
#include "instruction.hpp"
#include "decoder.hpp"
#include "regfile.hpp"
#include "alu.hpp"
#include "branch.hpp"
#include "loadstore.hpp"
#include "memory.hpp"

struct IFID {
	uint32_t pc = -1U, instr = 0;
	bool valid = false;

	void flush() { valid = false; }
};

struct IDEX {
	uint32_t pc = 0;
	Instruction instr;
	uint32_t r1 = 0, r2 = 0;
	bool valid = false;

	void flush() { valid = false; }
};

struct EXMEM {
	uint32_t pc = 0;
	Instruction instr;
	uint32_t alu = 0, r2 = 0;
	bool jumped = false;
	bool valid = false;

	void flush() { valid = false; jumped = false; }
};

struct MEMWB {
	uint32_t pc = 0;
	Instruction instr;
	uint32_t alu = 0, mem = 0;
	bool valid = false;

	void flush() { valid = false; }
};

struct PipelineControl {
	bool stall = false;
	bool flush = false;
	uint32_t target = 0;
	bool jumped = false;
};

class Pipeline {
public:
	Pipeline() = default;
	Pipeline(bool pipelined) : pipelined(pipelined) {}
	Pipeline(bool pipelined, bool forwarded) : pipelined(pipelined), forwarded(forwarded) {}

	IFID ifid;
	IDEX idex;
	EXMEM exmem;
	MEMWB memwb;

	static constexpr uint8_t WORD_BYTES = 4; // from CPU::WORD_BYTES?
	static constexpr int STAGES = 5;

	bool isPipelined() const { return pipelined; }

	bool fetch(uint32_t pc, Memory &mem) {
		if (ifid.valid) return false;
		ifid = {pc, mem.loadw(pc), true};
		return true;
	}

	bool decode(const RegisterFile &regs) {
		if (!ifid.valid) return false;

		Instruction instr = Decoder::decode(ifid.instr);

		uint32_t r1 = regs.read(instr.rs1);
		uint32_t r2 = regs.read(instr.rs2);
		
		if (forwarded) {
			r1 = forward(instr.rs1, r1);
			r2 = forward(instr.rs2, r2);
		}

		idex = {ifid.pc, instr, r1, r2, true};
		ifid.valid = false;
		return true;
	}

	void execute(ALU &alu, BranchUnit &bru) {
		if (!idex.valid) return;

		Op op = idex.instr.op;
		uint32_t imm = idex.instr.imm, r1 = idex.r1, r2 = idex.r2;
		
		bool jumped = false;
		uint32_t alu_out = 0;

		if      (op == LUI)   alu_out = imm;
		else if (op == AUIPC) alu_out = idex.pc + imm;

		else if (isALU(op))                 alu_out = alu.exec(op, r1, r2);
		else if (isALUI(op))                alu_out = alu.exec(op, r1, imm);
		else if (isLoad(op) || isStore(op)) alu_out = r1 + imm;
		
		else if (isBranch(op)) {
			jumped = bru.exec(op, r1, r2);
			r2 = idex.pc + imm;  // target in r2
		}
		
		else if (isJAL(op)) {
			jumped = true;
			alu_out = idex.pc + WORD_BYTES; // return address in alu
			r2 = (op == JAL) ? idex.pc + imm : (r1 + imm) & ~1u;  // target in r2
		}

		exmem = {idex.pc, idex.instr, alu_out, r2, jumped, true};
		idex.valid = false;
	}

	void memory(LoadStoreUnit &lsu) {
		if (!exmem.valid) return;

		Op op = exmem.instr.op;
		uint32_t addr = exmem.alu;
		uint32_t val = exmem.r2;

		if (isLoad(op)) val = lsu.load(op, addr);
		else if (isStore(op)) lsu.store(op, addr, val);

		memwb = {exmem.pc, exmem.instr, addr, val, true};
		exmem.valid = false;
	}

	bool writeback(RegisterFile &regs) {
		if (!memwb.valid) return false;

		Op op = memwb.instr.op;

		if (writesRegister(memwb.instr.op)) {
			uint8_t rd = memwb.instr.rd;
			if (isLoad(op)) regs.write(rd, memwb.mem);
			else 			regs.write(rd, memwb.alu);
		}

		memwb.valid = false;
		return (op == EBREAK);
	}

	void flush() {
		ifid.flush();
		idex.flush();
		exmem.jumped = false;
	}

	PipelineControl getControl() const {
		PipelineControl ctrl;

		ctrl.stall = hasDataHazard();
		
		if (exmem.valid && exmem.jumped) {
			ctrl.flush = true;
			ctrl.jumped = true;
			ctrl.target = exmem.r2;  // target in r2 for jumps
		}

		return ctrl;
	}

private:

	bool pipelined = true;
	bool forwarded = true; // TODO: make configurable

	uint32_t forward(uint8_t rs, uint32_t regVal) const {
		if (rs == 0) return regVal;
		
		// EX/MEM: forward ALU result (not loads - data not ready yet)
		if (exmem.valid && !isLoad(exmem.instr.op) && exmem.instr.rd == rs)
			return exmem.alu;
		
		// MEM/WB: forward load data or ALU result
		if (memwb.valid && writesRegister(memwb.instr.op) && memwb.instr.rd == rs)
			return isLoad(memwb.instr.op) ? memwb.mem : memwb.alu;
		
		return regVal;
	}
	
	bool hasDataHazard() const {
		if (!ifid.valid) return false;

		Instruction instr = Decoder::decode(ifid.instr);
		

		if (forwarded) {
			// load-use hazard
			if (idex.valid && isLoad(idex.instr.op) && hasDependency(instr, idex.instr)) return true;
		} else {
			// general data hazard
			if (idex.valid && hasDependency(instr, idex.instr)) return true;
			if (exmem.valid && hasDependency(instr, exmem.instr)) return true;
		}
		
		return false;
	}

	static bool hasDependency(const Instruction &consumer, const Instruction &producer) {
		if (!writesRegister(producer.op) || producer.rd == 0) return false;
		
		bool RS1_HAZARD = consumer.rs1 != 0 && producer.rd == consumer.rs1;
		bool RS2_HAZARD = consumer.rs2 != 0 && producer.rd == consumer.rs2;
		
		return RS1_HAZARD || RS2_HAZARD;
	}
};
