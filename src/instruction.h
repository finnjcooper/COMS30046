#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>

using namespace std;

enum Opcode {
	ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
	ADDI, SLTI, SLTIU, XORI, ORI, ANDI,
	SLLI, SRLI, SRAI,
	LB, LH, LW, LBU, LHU,
	SB, SH, SW,
	BEQ, BNE, BLT, BGE, BLTU, BGEU,
	JAL, JALR,
	LUI, AUIPC,
	ECALL, EBREAK,
	INVALID
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