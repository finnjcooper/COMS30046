#pragma once
#include "exec.hpp"

class ALU : ExecUnit {
public:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) override {
		
		if (isLoad(op) || isStore(op) || isUI(op)) return operand1 + operand2;

		switch (op) {
			case ADD: case ADDI:
				return operand1 + operand2;
			case SUB:
				return operand1 - operand2;
			case SLL: case SLLI:
				return operand1 << (operand2 & 0x1F);
			case SLT: case SLTI:
				return (static_cast<int32_t>(operand1) < static_cast<int32_t>(operand2)) ? 1 : 0;
			case SLTU: case SLTIU:
				return (operand1 < operand2) ? 1 : 0;
			case XOR: case XORI:
				return operand1 ^ operand2;
			case SRL: case SRLI:
				return operand1 >> (operand2 & 0x1F);
			case SRA: case SRAI:
				return static_cast<int32_t>(operand1) >> (operand2 & 0x1F);
			case OR: case ORI:
				return operand1 | operand2;
			case AND: case ANDI:
				return operand1 & operand2;
			default:
				return 0;
		}
	}
};
