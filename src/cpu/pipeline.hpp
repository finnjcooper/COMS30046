#pragma once
#include "instruction.hpp"
#include "decode.hpp"
#include <deque>

#define PIPELINE_WIDTH 4UL

struct IFID {
	uint32_t seq = 0, pc = -1U, instr = 0;
};

struct IDEX {
	uint32_t seq = 0, pc = 0;
	Instruction instr;
	uint32_t r1 = 0, r2 = 0;
};

struct EXMEM {
	uint32_t seq = 0, pc = 0;
	Instruction instr;
	uint32_t alu = 0, r2 = 0;
	bool jumped = false;
	bool should_halt = false;
};

struct PipelineControl {
	uint32_t target = 0;
	bool jumped = false;
	bool should_halt = false;
};

class Pipeline {
public:
	Pipeline(bool forwarding = true) : forwarding(forwarding) {}

	deque<IFID> ifids;
	deque<IDEX> idexs;
	deque<EXMEM> exmems;
	
	void flush() {
		ifids.clear();
		idexs.clear();
		exmems.clear();
	}

	uint32_t applyForwarding(uint8_t rs, uint32_t reg_val) const {
		if (!forwarding) return reg_val;
		return forward(rs, reg_val);
	}

	bool hasHazard(vector<Instruction> prevs, Instruction instr) const {
		for (auto &prev : prevs)
			if (hasDependency(instr, prev)) return true;

		if (forwarding) {
			// load-use hazard only
			for (auto &idex : idexs)
				if (isLoad(idex.instr.op) && hasDependency(instr, idex.instr)) return true;
			for (auto &exmem : exmems)
				if (isLoad(exmem.instr.op) && hasDependency(instr, exmem.instr)) return true;
		} else {
			// general data hazard
			for (auto &idex : idexs)
				if (hasDependency(instr, idex.instr)) return true;
			for (auto &exmem : exmems)
				if (hasDependency(instr, exmem.instr)) return true;
		}
		
		return false;
	}

private:
	bool forwarding;

	uint32_t forward(uint8_t rs, uint32_t reg_val) const {
		if (rs == 0) return reg_val;
		
		// loads aren't ready until writeback
		for (auto &exmem : exmems)
			if (!isLoad(exmem.instr.op) && exmem.instr.rd == rs)
				return exmem.alu;
		
		return reg_val;
	}

	static bool hasDependency(const Instruction &consumer, const Instruction &producer) {
		if (!writesRegister(producer.op) || producer.rd == 0) return false;
		
		bool RS1_HAZARD = consumer.rs1 != 0 && producer.rd == consumer.rs1;
		bool RS2_HAZARD = consumer.rs2 != 0 && producer.rd == consumer.rs2;
		
		return RS1_HAZARD || RS2_HAZARD;
	}
};
