#pragma once
#include "isa.hpp"

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
		case FLW:
			return true;
		default:
			return false;
	}
}

inline bool is_store(Op op) {
	switch (op) {
		case SB: case SH: case SW:
		case FSW:
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

inline bool is_mul(Op op) {
	switch (op) {
		case MUL: case MULH: case MULHSU: case MULHU:
			return true;
		default:
			return false;
	}
}

inline bool is_div(Op op) {
	switch (op) {
		case DIV: case DIVU: case REM: case REMU:
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
		case FLW:
		case FMADD_S: case FMSUB_S: case FNMADD_S: case FNMSUB_S:
		case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
		case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
		case FMIN_S: case FMAX_S: case FEQ_S: case FLT_S: case FLE_S:
		case FCVT_W_S: case FCVT_WU_S: case FCVT_S_W: case FCVT_S_WU:
		case FMV_X_W: case FMV_W_X:
		case FCLASS_S:
			return true;
		default:
			return false;
	}
}

inline size_t cycles(Op op) {
	if (is_load(op) || is_store(op)) return 2UL;
	if (is_mul(op)) return 3UL;
	if (is_div(op)) return 8UL;
	return 1UL;
}

inline int32_t sign_extend(uint32_t value, int bits) {
	int32_t shift = 32 - bits;
	return (int32_t)(value << shift) >> shift;
}
