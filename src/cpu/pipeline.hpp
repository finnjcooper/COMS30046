#pragma once
#include "instruction.hpp"
#include "decode.hpp"
#include <deque>

#define PIPELINE_WIDTH 4UL

struct DecodeEntry {
	uint32_t seq = 0, pc = -1U, instr = 0, tag = -1;
};

struct ExecEntry {
	uint32_t seq = 0, pc = 0;
	Instruction instr;
	uint32_t r1 = 0, r2 = 0;
	uint32_t tag = -1;
};

struct CommitEntry {
	uint32_t seq = 0, pc = 0;
	Instruction instr;
	uint32_t alu = 0, r2 = 0;
	bool jumped = false;
	bool should_halt = false;
	uint32_t tag = -1;
};

class Pipeline {
public:
	Pipeline(bool forwarding = true) : forwarding(forwarding) {}

	deque<DecodeEntry> decode_q;
	deque<ExecEntry> exec_q;
	deque<CommitEntry> commit_q;
	
	void flush() {
		decode_q.clear();
		exec_q.clear();
		commit_q.clear();
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
			for (auto &idex : exec_q)
				if (isLoad(idex.instr.op) && hasDependency(instr, idex.instr)) return true;
			for (auto &exmem : commit_q)
				if (isLoad(exmem.instr.op) && hasDependency(instr, exmem.instr)) return true;
		} else {
			// general data hazard
			for (auto &idex : exec_q)
				if (hasDependency(instr, idex.instr)) return true;
			for (auto &exmem : commit_q)
				if (hasDependency(instr, exmem.instr)) return true;
		}
		
		return false;
	}

private:
	bool forwarding;

	uint32_t forward(uint8_t rs, uint32_t reg_val) const {
		if (rs == 0) return reg_val;
		
		// loads aren't ready until writeback
		for (auto &exmem : commit_q)
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
