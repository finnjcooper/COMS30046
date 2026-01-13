#include "alu.h"

int ALU::execute(Opcode op, int operand1, int operand2) {
	switch (op) {
		case Opcode::ADD:
		case Opcode::ADDI:
			return operand1 + operand2;
		case Opcode::SUB:
			return operand1 - operand2;
		default:
			return 0;
	}
}