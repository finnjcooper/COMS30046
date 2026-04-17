#pragma once
#include <cstdint>
#include <stdexcept>

using namespace std;

enum Op {
	INVALID,
	// rv32i
	ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
	ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI,
	LB, LH, LW, LBU, LHU,
	SB, SH, SW,
	BEQ, BNE, BLT, BGE, BLTU, BGEU,
	JAL, JALR,
	LUI, AUIPC,
	ECALL,
	// rv32m
	MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
};

struct Instruction {
	Op op = INVALID;
	uint8_t rd, rs1, rs2;
	int32_t imm;
};


enum ExecType {
	LOGIC,
	MULDIV,
	CTRL,
	VECTOR,
	LOADSTORE,
};

inline ExecType exec_type(Op op) {
	switch (op) {
		case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
		case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
		case LUI: case AUIPC:
			return LOGIC;
		case MUL: case MULH: case MULHSU: case MULHU: case DIV: case DIVU: case REM: case REMU:
			return MULDIV;
		case BEQ: case BNE: case BLT: case BGE: case BLTU: case BGEU:
		case JAL: case JALR:
		case ECALL:
			return CTRL;
		case LB: case LH: case LW: case LBU: case LHU:
		case SB: case SH: case SW:
			return LOADSTORE;
		default:
			throw invalid_argument("Invalid operation");
	}
}
