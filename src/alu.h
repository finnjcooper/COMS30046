#pragma once
#include "instruction.h"

class ALU {
public:
	int execute(Opcode op, int operand1, int operand2);
};