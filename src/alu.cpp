#include "alu.h"

int ALU::execute(Opcode op, int operand1, int operand2) {
	switch (op) {
		case Opcode::ADD:
		case Opcode::ADDI:
			return operand1 + operand2;
		case Opcode::SUB:
		case Opcode::SUBI:
			return operand1 - operand2;
		case Opcode::CMP:
			return (operand1 == operand2) ? 0 : (operand1 < operand2) ? -1 : 1;
		default:
			return 0;
	}
}