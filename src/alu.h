#pragma once
#include "instruction.h"

class ALU {
public:
	int execute(Opcode op, uint32_t operand1, uint32_t operand2);
};