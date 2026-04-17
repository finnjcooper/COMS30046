#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>

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

enum ExecType {
	LOGIC,
	MULDIV,
	CTRL,
	VECTOR,
	LOADSTORE,
};

inline bool is_alu(Op op) {
	switch (op) {
		case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
			return true;
		default:
			return false;
	}
}

inline bool is_alui(Op op) {
	switch (op) {
		case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
			return true;
		default:
			return false;
	}
}

inline bool is_ui(Op op) {
	switch (op) {
		case LUI: case AUIPC:
			return true;
		default:
			return false;
	}
}

inline bool is_load(Op op) {
	switch (op) {
		case LB: case LH: case LW: case LBU: case LHU:
			return true;
		default:
			return false;
	}
}

inline bool is_store(Op op) {
	switch (op) {
		case SB: case SH: case SW:
			return true;
		default:
			return false;
	}
}

inline bool is_branch(Op op) {
	switch (op) {
		case BEQ: case BNE: case BLT: case BGE: case BLTU: case BGEU:
			return true;
		default:
			return false;
	}
}

inline bool is_jump(Op op) {
	switch (op) {
		case JAL: case JALR:
			return true;
		default:
			return false;
	}
}

inline bool is_control(Op op) {
	return is_branch(op) || is_jump(op) || op == ECALL;
}

inline bool writes_register(Op op) {
	switch (op) {
		case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
		case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
		case LB: case LH: case LW: case LBU: case LHU:
		case JAL: case JALR:
		case LUI: case AUIPC:
		case MUL: case MULH: case MULHSU: case MULHU: case DIV: case DIVU: case REM: case REMU:
			return true;
		default:
			return false;
	}
}

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

struct Instruction {
	Op op = INVALID;
	uint8_t rd, rs1, rs2;
	int32_t imm;
};

inline int32_t sign_extend(uint32_t value, int bits) {
	int32_t shift = 32 - bits;
	return (int32_t)(value << shift) >> shift;
}
