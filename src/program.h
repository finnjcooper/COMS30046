#pragma once
#include <vector>

using namespace std;

enum Opcode {
	ADD, ADDI,
	SUB, SUBI,
	CMP,
	LD, LDI, ST, STI,
	JMP, BR, BLT, BGT, BEQ,
	HALT
};

struct Instruction {
	Opcode op;
	int rd;
	int rs;
	int imm;
};

struct Program {
	vector<Instruction> instructions;
};

class Loader {
public:
	static Program sampleProgram();
};