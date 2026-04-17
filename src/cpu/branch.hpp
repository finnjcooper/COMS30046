#pragma once
#include "isa.hpp"

class BranchPredictor {
public:
	virtual bool predict(uint32_t pc, Instruction instr) = 0;
	virtual void update(uint32_t pc, bool taken) = 0;
};

class StaticBranchPredictor : public BranchPredictor {
public:
	StaticBranchPredictor(bool should_take = true) : should_take(should_take) {}

	bool predict(uint32_t pc, Instruction instr) override {
		if (instr.op == JAL) return true;
		if (!is_branch(instr.op)) return false;
		return should_take;
	}

	void update(uint32_t pc, bool taken) override {}

private:
	bool should_take;
};

class OneBitPredictor : public BranchPredictor {
public:
	OneBitPredictor(size_t size = 256, bool init_taken = false) : history(size, init_taken) {}

	bool predict(uint32_t pc, Instruction instr) override {
		if (instr.op == JAL) return true;
		if (!is_branch(instr.op)) return false;
		return history[normalise(pc)];
	}

	void update(uint32_t pc, bool taken) override {
		history[normalise(pc)] = taken;
	}

private:
	vector<bool> history;
	size_t normalise(uint32_t pc) {
		return (pc >> 2) % history.size();
	}
};

class TwoBitPredictor : public BranchPredictor {
public:
	TwoBitPredictor(size_t size = 256, uint8_t init_state = 1U) : history(size, init_state) {}

	bool predict(uint32_t pc, Instruction instr) override {
		if (instr.op == JAL) return true;
		if (!is_branch(instr.op)) return false;
		return history[normalise(pc)] >= 2;
	}

	void update(uint32_t pc, bool taken) override {
		uint8_t &state = history[normalise(pc)];
		if (taken) {
			if (state < 3) state++;
		} else {
			if (state > 0) state--;
		}
	}

private:
	vector<uint8_t> history;
	size_t normalise(uint32_t pc) {
		return (pc >> 2) % history.size();
	}
};
