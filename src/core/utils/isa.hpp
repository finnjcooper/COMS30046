#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

using namespace std;

static constexpr size_t MEM_SIZE = 64 * 1024ULL;
static constexpr uint8_t WORD_BYTES = 4ULL, NUM_REGISTERS = 32ULL, NUM_FLOAT_REGISTERS = 32ULL;
static constexpr uint8_t NUM_VECTOR_REGISTERS = 32ULL, MAX_VECTOR_LANES = 64ULL;

struct Value {
	bool vector_value = false;
	array<uint32_t, MAX_VECTOR_LANES> lanes {};

	Value(uint32_t scalar = 0) {
		lanes[0] = scalar;
	}

	static Value scalar(uint32_t scalar) {
		return Value(scalar);
	}

	static Value vector_zero() {
		Value value;
		value.vector_value = true;
		return value;
	}

	bool is_vector() const { return vector_value; }
	uint32_t as_scalar() const { return lanes[0]; }
	uint32_t lane(size_t index) const { return lanes[index]; }
	void set_lane(size_t index, uint32_t value) {
		vector_value = true;
		lanes[index] = value;
	}
};

struct VectorState {
	uint32_t tag = -1U;
	uint32_t vector_bits = 128;
	uint8_t vsew_bits = 32;
	uint8_t vl = 4;

	VectorState(uint32_t bits = 128) : vector_bits(bits) {
		vl = vlmax();
	}

	uint8_t vlmax() const {
		return vlmax(vsew_bits);
	}

	uint8_t vlmax(uint8_t sew_bits) const {
		return static_cast<uint8_t>(vector_bits / sew_bits);
	}

	uint8_t vl_for(uint32_t avl, uint8_t sew_bits) const {
		uint8_t max_vl = vlmax(sew_bits);
		return static_cast<uint8_t>(avl < max_vl ? avl : max_vl);
	}

	void apply(uint8_t next_vl, uint8_t sew_bits) {
		vsew_bits = sew_bits;
		vl = next_vl;
	}
};

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
	VSETVLI, VSETIVLI,
	VLE8_V, VLE16_V, VLE32_V,
	VSE8_V, VSE16_V, VSE32_V,
	VMUL_VV, VMACC_VV, VMACC_VX,
	VMV_V_I, VMV_V_X, VMV_S_X, VMV_X_S, VMV1R_V,
	VREDSUM_VS,
	VFMV_V_F, VFMACC_VV, VFMACC_VF,
};

struct Instruction {
	Op op = INVALID;
	uint8_t rd = 0, rs1 = 0, rs2 = 0;
	int32_t imm = 0;
	uint8_t rs3 = 0;
	uint8_t rm = 0;
	uint8_t sew = 0;
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
			return ExecType::CTRL;
		case LB: case LH: case LW: case LBU: case LHU:
		case SB: case SH: case SW:
		case FLW: case FSW:
		case VLE8_V: case VLE16_V: case VLE32_V:
		case VSE8_V: case VSE16_V: case VSE32_V:
			return ExecType::LOADSTORE;
		case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
		case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
		case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
		case FMIN_S: case FMAX_S: case FEQ_S: case FLT_S: case FLE_S:
		case FCVT_W_S: case FCVT_WU_S: case FCVT_S_W: case FCVT_S_WU:
		case FMV_X_W: case FMV_W_X:
		case FCLASS_S:
			return ExecType::FLOAT;
		case VSETVLI: case VSETIVLI:
		case VMUL_VV: case VMACC_VV: case VMACC_VX:
		case VMV_V_I: case VMV_V_X: case VMV_S_X: case VMV_X_S: case VMV1R_V:
		case VREDSUM_VS:
		case VFMV_V_F: case VFMACC_VV: case VFMACC_VF:
			return ExecType::VECTOR;
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
				case VSETVLI:
				case VLE8_V: case VLE16_V: case VLE32_V:
				case VSE8_V: case VSE16_V: case VSE32_V:
				case VMV_V_X: case VMV_S_X:
					return RegType::INT;
				case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
				case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
				case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
				case FMIN_S: case FMAX_S: case FEQ_S: case FLT_S: case FLE_S:
				case FCVT_W_S: case FCVT_WU_S:
				case FMV_X_W:
				case FCLASS_S:
				case VFMV_V_F:
					return RegType::FLOAT;
				case VMUL_VV: case VMACC_VV: case VMACC_VX:
				case VMV_X_S: case VMV1R_V:
				case VREDSUM_VS:
				case VFMACC_VV: case VFMACC_VF:
					return RegType::VECTOR;
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
				case VMUL_VV: case VMACC_VV:
				case VREDSUM_VS:
				case VFMACC_VV:
				case VSE8_V: case VSE16_V: case VSE32_V:
					return RegType::VECTOR;
				case VMACC_VX:
					return RegType::INT;
				case VFMACC_VF:
					return RegType::FLOAT;
				default:
					return RegType::NONE;
			}
		case 2:
			switch (op) {
				case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
					return RegType::FLOAT;
				case VMUL_VV: case VMACC_VV: case VMACC_VX:
				case VMV_V_I: case VMV_V_X: case VMV_S_X:
				case VREDSUM_VS:
				case VFMV_V_F:
				case VFMACC_VV: case VFMACC_VF:
					return RegType::VECTOR;
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
		case VSETVLI: case VSETIVLI:
		case VMV_X_S:
			return RegType::INT;
		case FADD_S: case FSUB_S: case FMUL_S: case FDIV_S: case FSQRT_S:
		case FMADD_S: case FMSUB_S: case FNMSUB_S: case FNMADD_S:
		case FSGNJ_S: case FSGNJN_S: case FSGNJX_S:
		case FMIN_S: case FMAX_S:
		case FLW:
		case FCVT_S_W: case FCVT_S_WU:
		case FMV_W_X:
			return RegType::FLOAT;
		case VLE8_V: case VLE16_V: case VLE32_V:
		case VMUL_VV: case VMACC_VV: case VMACC_VX:
		case VMV_V_I: case VMV_V_X: case VMV_S_X: case VMV1R_V:
		case VREDSUM_VS:
		case VFMV_V_F: case VFMACC_VV: case VFMACC_VF:
			return RegType::VECTOR;
		default:
			return RegType::NONE;
	}
}
