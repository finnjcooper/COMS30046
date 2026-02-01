#pragma once
#include "instruction.hpp"
#include "decoder.hpp"

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
	Pipeline(bool forwarding) : forwarding(forwarding) {}

	IFID ifid;
	IDEX idex;
	EXMEM exmem;
	MEMWB memwb;
	
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
			ctrl.target = exmem.r2;
		}
		return ctrl;
	}

	uint32_t applyForwarding(uint8_t rs, uint32_t regVal) const {
		if (!forwarding) return regVal;
		return forward(rs, regVal);
	}


private:
	bool forwarding = true;

	uint32_t forward(uint8_t rs, uint32_t regVal) const {
		if (rs == 0) return regVal;
		
		// EX/MEM: forward ALU result (not loads)
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
