#pragma once
#include "exec.hpp"

class BranchUnit : ExecUnit {
public:
	uint32_t exec(Op op, uint32_t val1, uint32_t val2) override {
		switch (op) {
			case BEQ:
				return val1 == val2;
			case BNE:
				return val1 != val2;
			case BLT:
				return static_cast<int32_t>(val1) < static_cast<int32_t>(val2);
			case BGE:
				return static_cast<int32_t>(val1) >= static_cast<int32_t>(val2);
			case BLTU:
				return val1 < val2;
			case BGEU:
				return val1 >= val2;
			default:
				return 0;
		}
	}
};
