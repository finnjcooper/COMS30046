#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>

using namespace std;

enum Opcode {
	ADD, ADDI,
	SUB, SUBI,
	CMP,
	LD, LDI, ST, STI,
	JMP, BR, BLT, BGT, BEQ,
	HALT, NOP
};

struct Instruction {
	Opcode op;
	uint8_t rd;
	uint8_t rs1;
	uint8_t rs2;
	uint32_t imm;
};

class Loader {
public:
	static vector<Instruction> sampleProgram();
	static vector<uint8_t> fromBinary(const string& filename);
};