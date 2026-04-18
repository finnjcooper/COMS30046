#pragma once
#include "exec.hpp"

class VectorUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		// TODO: implement vector instructions
		busy_ = false;
		return ExecEntry {current.op, 0, 0, 0, false, false, current.tag};
	}

private:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) override { return 0; }
};
