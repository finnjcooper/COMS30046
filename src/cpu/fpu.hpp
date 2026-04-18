#pragma once
#include "exec.hpp"

class FloatingPointUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		uint32_t r1 = current.Vj, r2 = current.Vk;

		uint32_t float_out = exec(op, r1, r2);
		busy_ = false;
		return ExecEntry {current.op, float_out, 0, 0, false, false, current.tag};
	}

private:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) override {
		switch (op) {
			case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
			case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
			case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
			case FMIN_S: case FMAX_S:
			default: return 0;
		}
	}
};
