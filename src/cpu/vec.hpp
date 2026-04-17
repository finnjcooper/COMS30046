#pragma once
#include "exec.hpp"

class VectorUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		uint32_t r1 = current.Vj, r2 = current.Vk;

		uint32_t vec_out = exec(op, r1, r2);
		busy_ = false;
		return ExecEntry {current.op, vec_out, 0, 0, false, false, current.tag};
	}

private:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) override {
		switch (op) {}
		return 0;
	}
};
