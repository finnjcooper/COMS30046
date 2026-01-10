#include "program.h"

Program Loader::sampleProgram() {
	Program prog;
	prog.instructions = {
		{LDI, 1, 0},    // Load immediate value 0 into R1
		{LDI, 2, 10},   // Load immediate value 10 into R2
		{ADD, 3, 1},    // R3 = R1 + R2
		{STI, 3, 100},  // Store value of R3 into memory address 100
		{LD, 4, 100},   // Load value from memory address 100 into R4
		{SUBI, 4, 5},   // R4 = R4 - 5
		{BEQ, 0, 8},    // If R4 == 0, branch to instruction at index 8
		{JMP, 0, 2},    // Jump back to instruction at index 2
		{HALT, 0, 0}    // Halt execution
	};

	return prog;
};