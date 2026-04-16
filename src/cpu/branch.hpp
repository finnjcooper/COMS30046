#pragma once
#include "instruction.hpp"

class BranchPredictor {
public:
	virtual bool predict(uint32_t pc, Instruction instr) = 0;
	virtual void update(uint32_t pc, bool taken) = 0;
};

class StaticBranchPredictor : public BranchPredictor {
public:
	StaticBranchPredictor(bool should_take = true) : should_take(should_take) {}

	bool predict(uint32_t pc, Instruction instr) override {
		if (is_jump(instr.op)) return true;
		return should_take;
	}

	void update(uint32_t pc, bool taken) override {}

private:
	bool should_take;
};
