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
		case FLW: case VLE8_V: case VLE16_V: case VLE32_V:
			return true;
		default:
			return false;
	}
}

inline bool is_store(Op op) {
	switch (op) {
		case SB: case SH: case SW:
		case FSW: case VSE8_V: case VSE16_V: case VSE32_V:
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

inline bool is_fadd(Op op) {
	switch (op) {
		case FADD_S: case FSUB_S: case FMIN_S: case FMAX_S:
		case FCVT_W_S: case FCVT_WU_S: case FCVT_S_W: case FCVT_S_WU:
			return true;
		default:
			return false;
	}
}

inline bool is_fmul(Op op) {
	switch (op) {
		case FMUL_S:
		case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
			return true;
		default:
			return false;
	}
}

inline bool is_vset(Op op) {
	return op == VSETVLI || op == VSETIVLI;
}

inline bool is_vmul(Op op) {
	switch(op) {
		case VMUL_VV: case VMUL_VX: case VMACC_VV: case VMADD_VV: case VREDSUM_VS:
			return true;
		default:
			return false;
	}
}

inline bool is_vfmul(Op op) {
	switch(op) {
		case VFMACC_VV: case VFMADD_VV: return true;
		default: return false;
	}
}

inline bool is_vload(Op op) {
	switch (op) {
		case VLE8_V: case VLE16_V: case VLE32_V: return true;
		default: return false;
	}
}

inline bool is_vstore(Op op) {
	switch (op) {
		case VSE8_V: case VSE16_V: case VSE32_V: return true;
		default: return false;
	}
}

inline bool is_vec(Op op) {
	return exec_type(op) == ExecType::VECTOR || is_vload(op) || is_vstore(op);
}

inline bool is_ctrl(Op op) {
	return is_branch(op) || is_jump(op) || op == ECALL;
}

inline bool writes_register(Op op) {
	return dst_type(op) != RegType::NONE;
}

inline size_t cycles(Op op) {
	if (exec_type(op) == ExecType::LOADSTORE) return 2UL;
	if (is_vmul(op)) return 3UL;
	if (is_vfmul(op)) return 4UL;
	if (is_mul(op)) return 3UL;
	if (is_div(op)) return 8UL;
	if (is_fadd(op)) return 3UL;
	if (is_fmul(op)) return 4UL;
	if (op == FDIV_S) return 10UL;
	if (op == FSQRT_S) return 12UL;
	return 1UL;
}

inline uint8_t op_sew(Op op) {
	switch (op) {
		case VLE8_V: case VSE8_V: return 8;
		case VLE16_V: case VSE16_V: return 16;
		case VLE32_V: case VSE32_V: return 32;
		default: return 0;
	}
}

inline int32_t sign_extend(uint32_t value, int bits) {
	int32_t shift = 32 - bits;
	return (int32_t)(value << shift) >> shift;
}
