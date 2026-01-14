#pragma once
#include "instruction.h"

class ALU {
public:
	int execute(Opcode op, uint32_t operand1, uint32_t operand2) {
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
			default:
				return 0;
		}
	}
};
