#pragma once

class BranchUnit {
public:
	bool evaluate(Op op, int32_t val1, int32_t val2) {
		switch (op) {
			case BEQ:
				return val1 == val2;
			case BNE:
				return val1 != val2;
			case BLT:
				return val1 < val2;
			case BGE:
				return val1 >= val2;
			case BLTU:
				return static_cast<uint32_t>(val1) < static_cast<uint32_t>(val2);
			case BGEU:
				return static_cast<uint32_t>(val1) >= static_cast<uint32_t>(val2);
			default:
				return false;
		}
	}
};
