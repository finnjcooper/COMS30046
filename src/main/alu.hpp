#pragma once
#include "exec.hpp"

class ALU : ExecUnit {
public:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) override {
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
			case MUL: case MULH: {
				int64_t result = static_cast<int64_t>(static_cast<int32_t>(operand1)) * static_cast<int64_t>(static_cast<int32_t>(operand2));
				return op == MUL ? static_cast<uint32_t>(result) : static_cast<uint32_t>(result >> 32);
			}
			case MULHSU: {
				int64_t result = static_cast<int64_t>(static_cast<int32_t>(operand1)) * static_cast<uint64_t>(operand2);
				return static_cast<uint32_t>(result >> 32);
			}
			case MULHU: {
				uint64_t result = static_cast<uint64_t>(operand1) * static_cast<uint64_t>(operand2);
				return static_cast<uint32_t>(result >> 32);
			}
			case DIV:
				if (operand2 == 0) return 0xFFFFFFFF;
				return static_cast<uint32_t>(static_cast<int32_t>(operand1) / static_cast<int32_t>(operand2));
			case REM:
				if (operand2 == 0) return operand1;
				return static_cast<uint32_t>(static_cast<int32_t>(operand1) % static_cast<int32_t>(operand2));
			case DIVU:
				if (operand2 == 0) return 0xFFFFFFFF;
				return operand1 / operand2;
			case REMU:
				if (operand2 == 0) return operand1;
				return operand1 % operand2;
			default:
				return 0;
		}
	}
};
