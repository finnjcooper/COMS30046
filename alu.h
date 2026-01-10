#pragma once
#include "program.h"

class ALU {
public:
	int execute(Opcode op, int operand1, int operand2);
};