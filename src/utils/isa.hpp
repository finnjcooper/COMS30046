#pragma once
#include <cstdint>
#include <stdexcept>

using namespace std;

static constexpr size_t MEM_SIZE = 64 * 1024ULL;
static constexpr uint8_t WORD_BYTES = 4ULL, NUM_REGISTERS = 32ULL, NUM_FLOAT_REGISTERS = 32ULL;

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
	//rv32f
	FADD_S, FSUB_S, FMUL_S, FDIV_S, FSQRT_S,
	FMADD_S, FMSUB_S, FNMSUB_S, FNMADD_S,
	FSGNJ_S, FSGNJN_S, FSGNJX_S,
	FMIN_S, FMAX_S, FEQ_S, FLT_S, FLE_S,
	FLW, FSW, FMV_X_W, FMV_W_X,
	FCVT_W_S, FCVT_WU_S, FCVT_S_W, FCVT_S_WU,
	FCLASS_S,
	//rv32v
	//TODO: add vector instructions
};

struct Instruction {
	Op op = INVALID;
	uint8_t rd = 0, rs1 = 0, rs2 = 0;
	int32_t imm = 0;
	uint8_t rs3 = 0;
	uint8_t rm = 0;
};


enum class ExecType {
	LOGIC,
	MULDIV,
	CTRL,
	FLOAT,
	VECTOR,
	LOADSTORE,
};

inline ExecType exec_type(Op op) {
	switch (op) {
		case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
		case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
		case LUI: case AUIPC:
			return ExecType::LOGIC;
		case MUL: case MULH: case MULHSU: case MULHU: case DIV: case DIVU: case REM: case REMU:
			return ExecType::MULDIV;
		case BEQ: case BNE: case BLT: case BGE: case BLTU: case BGEU:
		case JAL: case JALR:
		case ECALL:
			return ExecType::CTRL;
		case LB: case LH: case LW: case LBU: case LHU:
		case SB: case SH: case SW:
		case FLW: case FSW:
			return ExecType::LOADSTORE;
		case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
		case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
		case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
		case FMIN_S: case FMAX_S: case FEQ_S: case FLT_S: case FLE_S:
		case FCVT_W_S: case FCVT_WU_S: case FCVT_S_W: case FCVT_S_WU:
		case FMV_X_W: case FMV_W_X:
		case FCLASS_S:
			return ExecType::FLOAT;
		default:
			throw invalid_argument("Invalid operation");
	}
}

enum class RegType {
	NONE,
	INT,
	FLOAT,
	VECTOR,
};

inline RegType src_type(Op op, uint8_t operand) {
	switch (operand) {
		case 0:
			switch (op) {
				case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
				case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
				case LB: case LH: case LW: case LBU: case LHU:
				case SB: case SH: case SW:
				case BEQ: case BNE: case BLT: case BGE: case BLTU: case BGEU:
				case JALR:
				case MUL: case MULH: case MULHSU: case MULHU: case DIV: case DIVU: case REM: case REMU:
				case FLW: case FSW:
				case FCVT_S_W: case FCVT_S_WU:
				case FMV_W_X:
					return RegType::INT;
				case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
				case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
				case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
				case FMIN_S: case FMAX_S: case FEQ_S: case FLT_S: case FLE_S:
				case FCVT_W_S: case FCVT_WU_S:
				case FMV_X_W:
				case FCLASS_S:
					return RegType::FLOAT;
				default:
					return RegType::NONE;
			}
		case 1:
			switch (op) {
				case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
				case SB: case SH: case SW:
				case BEQ: case BNE: case BLT: case BGE: case BLTU: case BGEU:
				case MUL: case MULH: case MULHSU: case MULHU: case DIV: case DIVU: case REM: case REMU:
					return RegType::INT;
				case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S:
				case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
				case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
				case FMIN_S: case FMAX_S: case FEQ_S: case FLT_S: case FLE_S:
				case FSW:
					return RegType::FLOAT;
				default:
					return RegType::NONE;
			}
		case 2:
			switch (op) {
				case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
					return RegType::FLOAT;
				default:
					return RegType::NONE;
			}
		default:
			return RegType::NONE;
	}
}

inline RegType dst_type(Op op) {
	switch (op) {
		case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND:
		case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
		case LB: case LH: case LW: case LBU: case LHU:
		case JAL: case JALR:
		case LUI: case AUIPC:
		case MUL: case MULH: case MULHSU: case MULHU: case DIV: case DIVU: case REM: case REMU:
		case FEQ_S: case FLT_S: case FLE_S:
		case FCVT_W_S: case FCVT_WU_S:
		case FMV_X_W:
		case FCLASS_S:
			return RegType::INT;
		case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
		case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
		case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
		case FMIN_S: case FMAX_S:
		case FLW:
		case FCVT_S_W: case FCVT_S_WU:
		case FMV_W_X:
			return RegType::FLOAT;
		default:
			return RegType::NONE;
	}
}
