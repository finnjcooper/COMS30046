#pragma once
#include <vector>
#include "helpers.hpp"

struct Prediction {
	bool taken = false;
	uint32_t target = 0;
};

class BranchPredictor {
public:
	BranchPredictor(size_t btb_size = 256) : btb(btb_size) {}
	virtual ~BranchPredictor() = default;

	virtual void clear() {
		fill(btb.begin(), btb.end(), BTBEntry());
	}

	Prediction predict(uint32_t pc, const Instruction &instr) {
		uint32_t fallthrough = pc + WORD_BYTES;

		if (instr.op == JAL) return {true, static_cast<uint32_t>(pc + instr.imm)};

		if (is_branch(instr.op) && !predict_direction(pc, instr))
			return {false, fallthrough};

		const auto &entry = btb[btb_index(pc)];
		if (entry.pc == pc) return {true, entry.target};
		if (is_branch(instr.op)) return {true, static_cast<uint32_t>(pc + instr.imm)};
		return {false, fallthrough};
	}

	void update(uint32_t pc, Op op, bool taken, uint32_t target) {
		if (is_branch(op)) update_direction(pc, taken);

		if (taken) {
			auto &entry = btb[btb_index(pc)];
			entry = {pc, target};
		}
	}

protected:
	virtual bool predict_direction(uint32_t, const Instruction &) = 0;
	virtual void update_direction(uint32_t, bool) = 0;

private:
	struct BTBEntry {
		uint32_t pc = -1U;
		uint32_t target = 0;
	};

	vector<BTBEntry> btb;

	size_t btb_index(uint32_t pc) const {
		return (pc >> 2) % btb.size();
	}
};

class StaticBranchPredictor : public BranchPredictor {
public:
	StaticBranchPredictor(bool should_take = true) : should_take(should_take) {}

	bool predict_direction(uint32_t, const Instruction &) override {
		return should_take;
	}

	void update_direction(uint32_t, bool) override {}

private:
	bool should_take;
};

class BTFNT : public BranchPredictor {
public:
	bool predict_direction(uint32_t, const Instruction &instr) override {
		return instr.imm < 0;
	}

	void update_direction(uint32_t, bool) override {}
};

class OneBitPredictor : public BranchPredictor {
public:
	OneBitPredictor(size_t size = 256, bool init_taken = false) : history(size, init_taken), init_taken(init_taken) {}

	void clear() override {
		BranchPredictor::clear();
		fill(history.begin(), history.end(), init_taken);
	}

	bool predict_direction(uint32_t pc, const Instruction &) override {
		return history[index(pc)];
	}

	void update_direction(uint32_t pc, bool taken) override {
		history[index(pc)] = taken;
	}

private:
	vector<bool> history;
	bool init_taken;
	size_t index(uint32_t pc) {
		return (pc >> 2) % history.size();
	}
};

class TwoBitPredictor : public BranchPredictor {
public:
	TwoBitPredictor(size_t size = 256, uint8_t init_state = 1U) : history(size, init_state), init_state(init_state) {}

	void clear() override {
		BranchPredictor::clear();
		fill(history.begin(), history.end(), init_state);
	}

	bool predict_direction(uint32_t pc, const Instruction &) override {
		return history[index(pc)] >= 2;
	}

	void update_direction(uint32_t pc, bool taken) override {
		uint8_t &state = history[index(pc)];
		if (taken) {
			if (state < 3) state++;
		} else if (state > 0) {
			state--;
		}
	}

private:
	vector<uint8_t> history;
	uint8_t init_state;
	size_t index(uint32_t pc) {
		return (pc >> 2) % history.size();
	}
};
