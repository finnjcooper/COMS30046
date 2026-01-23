#pragma once

class BranchUnit {
public:
	bool evaluate(Opcode op, int32_t val1, int32_t val2) {
		switch (op) {
			case Opcode::BEQ:
				return val1 == val2;
			case Opcode::BNE:
				return val1 != val2;
			case Opcode::BLT:
				return val1 < val2;
			case Opcode::BGE:
				return val1 >= val2;
			case Opcode::BLTU:
				return static_cast<uint32_t>(val1) < static_cast<uint32_t>(val2);
			case Opcode::BGEU:
				return static_cast<uint32_t>(val1) >= static_cast<uint32_t>(val2);
			default:
				return false;
		}
	}
};
