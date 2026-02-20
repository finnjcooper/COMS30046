#pragma once
#include "instruction.hpp"
#include "decode.hpp"

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
	bool should_halt = false;
	bool valid = false;

	void flush() { valid = false; jumped = false; should_halt = false; }
};

struct MEMWB {
	uint32_t pc = 0;
	Instruction instr;
	uint32_t alu = 0, mem = 0;
	bool should_halt = false;
	bool valid = false;

	void flush() { valid = false; }
};

struct PipelineControl {
	bool stall = false;
	uint32_t target = 0;
	bool jumped = false;
	bool should_halt = false;
};

class Pipeline {
public:
	Pipeline(bool forwarding = true) : forwarding(forwarding) {}

	IFID ifid;
	IDEX idex;
	EXMEM exmem;
	MEMWB memwb;
	
	void flush() {
		ifid.flush();
		idex.flush();
	}

	PipelineControl getControl() const {
		PipelineControl ctrl;
		ctrl.stall = hasDataHazard();

		if (!exmem.valid) return ctrl;
		if (exmem.should_halt) ctrl.should_halt = true;
		if (exmem.jumped && !ctrl.jumped) {
			ctrl.jumped = true;
			ctrl.target = exmem.r2;
		}

		return ctrl;
	}

	uint32_t applyForwarding(uint8_t rs, uint32_t reg_val) const {
		if (!forwarding) return reg_val;
		return forward(rs, reg_val);
	}

private:
	bool forwarding;

	uint32_t forward(uint8_t rs, uint32_t reg_val) const {
		if (rs == 0) return reg_val;
		
		// EX/MEM: forward ALU result (not loads)
		if (exmem.valid && !isLoad(exmem.instr.op) && exmem.instr.rd == rs)
			return exmem.alu;
		
		// MEM/WB: forward load data or ALU result
		if (memwb.valid && writesRegister(memwb.instr.op) && memwb.instr.rd == rs)
			return isLoad(memwb.instr.op) ? memwb.mem : memwb.alu;
		
		return reg_val;
	}

	bool hasDataHazard() const {
		if (!ifid.valid) return false;

		Instruction instr = Decoder::decode(ifid.instr);

		if (forwarding) {
			// load-use hazard only
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
