#pragma once
#include "instruction.h"

class ALU {
public:
	uint32_t execute(Opcode op, uint32_t operand1, uint32_t operand2) {
		switch (op) {
			case Opcode::ADD: case Opcode::ADDI:
				return operand1 + operand2;
			case Opcode::SUB:
				return operand1 - operand2;
			case Opcode::SLL: case Opcode::SLLI:
				return operand1 << (operand2 & 0x1F);
			case Opcode::SLT: case Opcode::SLTI:
				return (static_cast<int32_t>(operand1) < static_cast<int32_t>(operand2)) ? 1 : 0;
			case Opcode::SLTU: case Opcode::SLTIU:
				return (operand1 < operand2) ? 1 : 0;
			case Opcode::XOR: case Opcode::XORI:
				return operand1 ^ operand2;
			case Opcode::SRL: case Opcode::SRLI:
				return operand1 >> (operand2 & 0x1F);
			case Opcode::SRA: case Opcode::SRAI:
				return static_cast<int32_t>(operand1) >> (operand2 & 0x1F);
			case Opcode::OR: case Opcode::ORI:
				return operand1 | operand2;
			case Opcode::AND: case Opcode::ANDI:
				return operand1 & operand2;
			case Opcode::MUL: case Opcode::MULH: {
				int64_t result = static_cast<int64_t>(static_cast<int32_t>(operand1)) * static_cast<int64_t>(static_cast<int32_t>(operand2));
				return op == Opcode::MUL ? static_cast<uint32_t>(result) : static_cast<uint32_t>(result >> 32);
			}
			case Opcode::MULHSU: {
				int64_t result = static_cast<int64_t>(static_cast<int32_t>(operand1)) * static_cast<uint64_t>(operand2);
				return static_cast<uint32_t>(result >> 32);
			}
			case Opcode::MULHU: {
				uint64_t result = static_cast<uint64_t>(operand1) * static_cast<uint64_t>(operand2);
				return static_cast<uint32_t>(result >> 32);
			}
			case Opcode::DIV:
				if (operand2 == 0) return 0xFFFFFFFF;
				return static_cast<uint32_t>(static_cast<int32_t>(operand1) / static_cast<int32_t>(operand2));
			case Opcode::REM:
				if (operand2 == 0) return operand1;
				return static_cast<uint32_t>(static_cast<int32_t>(operand1) % static_cast<int32_t>(operand2));
			case Opcode::DIVU:
				if (operand2 == 0) return 0xFFFFFFFF;
				return operand1 / operand2;
			case Opcode::REMU:
				if (operand2 == 0) return operand1;
				return operand1 % operand2;
			default:
				return 0;
		}
	}
};
