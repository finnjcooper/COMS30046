#pragma once
#include <vector>

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
	uint8_t rd, rs1, rs2;
	int32_t imm;
};

struct Program {
	vector<uint8_t> instrs;
	uint32_t entryPoint;
};
