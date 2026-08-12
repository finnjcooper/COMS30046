#pragma once
#include "helpers.hpp"

struct DecodeEntry {
	uint32_t pc = 0;
	Instruction instr;
	bool pred_taken = false;
	uint32_t pred_target = 0;
};

struct DecodeState {
	DecodeState() = default;
	DecodeState(const DecodeEntry &e) : pc(e.pc), op(e.instr.op), pred_taken(e.pred_taken), pred_target(e.pred_target) {}

	uint32_t pc = 0;
	Op op = INVALID;
	bool pred_taken = false;
	uint32_t pred_target = 0;
};

class Decoder {
public:
	static Instruction decode(uint32_t instruction) {
		uint8_t op = instruction & 0x7F;
		uint8_t f3 = (instruction >> 12) & 0x07;
		uint8_t f7 = (instruction >> 25) & 0x7F;
		uint8_t rd = (instruction >> 7) & 0x1F;
		uint8_t rs1 = (instruction >> 15) & 0x1F;
		uint8_t rs2 = (instruction >> 20) & 0x1F;
		uint32_t immu = instruction >> 20;
		
		switch (op) {
			case 0x33: {
				switch(f7) {
					case 0x20:
						switch (f3) {
							case 0x00: return {SUB, rd, rs1, rs2, 0};
							case 0x05: return {SRA, rd, rs1, rs2, 0};
						}
						return {};
					case 0x00:
						switch (f3) {
							case 0x00: return {ADD, rd, rs1, rs2, 0};
							case 0x01: return {SLL, rd, rs1, rs2, 0};
							case 0x02: return {SLT, rd, rs1, rs2, 0};
							case 0x03: return {SLTU, rd, rs1, rs2, 0};
							case 0x04: return {XOR, rd, rs1, rs2, 0};
							case 0x05: return {SRL, rd, rs1, rs2, 0};
							case 0x06: return {OR, rd, rs1, rs2, 0};
							case 0x07: return {AND, rd, rs1, rs2, 0};
						}
						return {};
					case 0x01:
						switch (f3) {
							case 0x00: return {MUL, rd, rs1, rs2, 0};
							case 0x01: return {MULH, rd, rs1, rs2, 0};
							case 0x02: return {MULHSU, rd, rs1, rs2, 0};
							case 0x03: return {MULHU, rd, rs1, rs2, 0};
							case 0x04: return {DIV, rd, rs1, rs2, 0};
							case 0x05: return {DIVU, rd, rs1, rs2, 0};
							case 0x06: return {REM, rd, rs1, rs2, 0};
							case 0x07: return {REMU, rd, rs1, rs2, 0};
						}
						return {};
				}
				return {};
			}
			case 0x13: {
				int32_t imm = sign_extend(immu, 12);
				switch(f3) {
					case 0x00: return {ADDI, rd, rs1, 0, imm};
					case 0x01: return f7 == 0x00 ? Instruction {SLLI, rd, rs1, 0, rs2} : Instruction {};
					case 0x02: return {SLTI, rd, rs1, 0, imm};
					case 0x03: return {SLTIU, rd, rs1, 0, imm};
					case 0x04: return {XORI, rd, rs1, 0, imm};
					case 0x05:
						if (f7 == 0x20) return {SRAI, rd, rs1, 0, rs2};
						if (f7 == 0x00) return {SRLI, rd, rs1, 0, rs2};
						return {};
					case 0x06: return {ORI, rd, rs1, 0, imm};
					case 0x07: return {ANDI, rd, rs1, 0, imm};
				}
				return {};
			}
			case 0x03: {
				int32_t imm = sign_extend(immu, 12);
				switch(f3) {
					case 0x00: return {LB,  rd, rs1, 0, imm};
					case 0x01: return {LH,  rd, rs1, 0, imm};
					case 0x02: return {LW,  rd, rs1, 0, imm};
					case 0x04: return {LBU, rd, rs1, 0, imm};
					case 0x05: return {LHU, rd, rs1, 0, imm};
				}
				return {};
			}
			case 0x07: {
				int32_t imm = sign_extend(immu, 12);
				if (f3 == 0x02) return {FLW, rd, rs1, 0, imm};
				if (!is_unit_stride_vector_mem(instruction)) return {};
				if (f3 == 0x00) return {VLE8_V, rd, rs1, 0, 0, 0, 0, 8};
				if (f3 == 0x05) return {VLE16_V, rd, rs1, 0, 0, 0, 0, 16};
				if (f3 == 0x06) return {VLE32_V, rd, rs1, 0, 0, 0, 0, 32};
				return {};
			}
			case 0x23: {
				int32_t imm = sign_extend((f7 << 5) | rd, 12);
				switch(f3) {
					case 0x00: return {SB, 0, rs1, rs2, imm};
					case 0x01: return {SH, 0, rs1, rs2, imm};
					case 0x02: return {SW, 0, rs1, rs2, imm};
				}
				return {};
			}
			case 0x27: {
				int32_t imm = sign_extend((f7 << 5) | rd, 12);
				if (f3 == 0x02) return {FSW, 0, rs1, rs2, imm};
				if (!is_unit_stride_vector_mem(instruction)) return {};
				if (f3 == 0x00) return {VSE8_V, 0, rs1, rd, 0, 0, 0, 8};
				if (f3 == 0x05) return {VSE16_V, 0, rs1, rd, 0, 0, 0, 16};
				if (f3 == 0x06) return {VSE32_V, 0, rs1, rd, 0, 0, 0, 32};
				return {};
			}
			case 0x43: {
				if (((instruction >> 25) & 0x03) != 0 || !valid_rm(f3)) return {};
				uint8_t rs3 = (instruction >> 27) & 0x1F;
				return {FMADD_S, rd, rs1, rs2, 0, rs3, f3};
			}
			case 0x47: {
				if (((instruction >> 25) & 0x03) != 0 || !valid_rm(f3)) return {};
				uint8_t rs3 = (instruction >> 27) & 0x1F;
				return {FMSUB_S, rd, rs1, rs2, 0, rs3, f3};
			}
			case 0x4B: {
				if (((instruction >> 25) & 0x03) != 0 || !valid_rm(f3)) return {};
				uint8_t rs3 = (instruction >> 27) & 0x1F;
				return {FNMSUB_S, rd, rs1, rs2, 0, rs3, f3};
			}
			case 0x4F: {
				if (((instruction >> 25) & 0x03) != 0 || !valid_rm(f3)) return {};
				uint8_t rs3 = (instruction >> 27) & 0x1F;
				return {FNMADD_S, rd, rs1, rs2, 0, rs3, f3};
			}
			case 0x53: {
				uint8_t fmt = f7 & 0x03;
				uint8_t f5 = f7 >> 2;
				if (fmt != 0) return {};

				switch (f5) {
					case 0x00:
						return valid_rm(f3) ? Instruction {FADD_S, rd, rs1, rs2, 0, 0, f3} : Instruction {};
					case 0x01:
						return valid_rm(f3) ? Instruction {FSUB_S, rd, rs1, rs2, 0, 0, f3} : Instruction {};
					case 0x02:
						return valid_rm(f3) ? Instruction {FMUL_S, rd, rs1, rs2, 0, 0, f3} : Instruction {};
					case 0x03:
						return valid_rm(f3) ? Instruction {FDIV_S, rd, rs1, rs2, 0, 0, f3} : Instruction {};
					case 0x04:
						if (f3 == 0x00) return {FSGNJ_S, rd, rs1, rs2, 0};
						if (f3 == 0x01) return {FSGNJN_S, rd, rs1, rs2, 0};
						if (f3 == 0x02) return {FSGNJX_S, rd, rs1, rs2, 0};
						return {};
					case 0x05:
						if (f3 == 0x00) return {FMIN_S, rd, rs1, rs2, 0};
						if (f3 == 0x01) return {FMAX_S, rd, rs1, rs2, 0};
						return {};
					case 0x0B:
						if (rs2 != 0x00 || !valid_rm(f3)) return {};
						return {FSQRT_S, rd, rs1, 0, 0, 0, f3};
					case 0x14:
						if (f3 == 0x00) return {FLE_S, rd, rs1, rs2, 0};
						if (f3 == 0x01) return {FLT_S, rd, rs1, rs2, 0};
						if (f3 == 0x02) return {FEQ_S, rd, rs1, rs2, 0};
						return {};
					case 0x18:
						if (!valid_rm(f3)) return {};
						if (rs2 == 0x00) return {FCVT_W_S, rd, rs1, 0, 0, 0, f3};
						if (rs2 == 0x01) return {FCVT_WU_S, rd, rs1, 0, 0, 0, f3};
						return {};
					case 0x1A:
						if (!valid_rm(f3)) return {};
						if (rs2 == 0x00) return {FCVT_S_W, rd, rs1, 0, 0, 0, f3};
						if (rs2 == 0x01) return {FCVT_S_WU, rd, rs1, 0, 0, 0, f3};
						return {};
					case 0x1C:
						if (rs2 != 0x00) return {};
						if (f3 == 0x00) return {FMV_X_W, rd, rs1, 0, 0};
						if (f3 == 0x01) return {FCLASS_S, rd, rs1, 0, 0};
						return {};
					case 0x1E:
						if (rs2 != 0x00 || f3 != 0x00) return {};
						return {FMV_W_X, rd, rs1, 0, 0};
					default:
						return {};
				}
			}
			case 0x57: {
				uint8_t vm = (instruction >> 25) & 0x01;
				uint8_t f6 = (instruction >> 26) & 0x3F;
				if (f3 == 0x07) {
					uint8_t vset = (instruction >> 30) & 0x3;
					uint32_t zimm = vset == 0x3
						? ((instruction >> 20) & 0x3FF)
						: ((instruction >> 20) & 0x7FF);
					uint8_t sew = sew_from_vtype(zimm);
					if (!sew) return {};
					if (vset == 0x3)
						return {VSETIVLI, rd, 0, 0, static_cast<int32_t>(rs1), 0, 0, sew};
					if (vset == 0x0) {
						int32_t avl = rs1 == 0 ? (rd == 0 ? -2 : -1) : 0;
						return {VSETVLI, rd, rs1, 0, avl, 0, 0, sew};
					} return {};
				} if (!vm) return {};

				switch (f6) {
					case 0x00:
						if (f3 == 0x02) return {VREDSUM_VS, rd, rs2, rs1, 0, rd};
						return {};
					case 0x10:
						if (f3 == 0x02 && rs1 == 0x00) return {VMV_X_S, rd, rs2, 0, 0};
						if (f3 == 0x06 && rs2 == 0x00) return {VMV_S_X, rd, rs1, 0, 0, rd};
						return {};
					case 0x17:
						switch(f3) {
							case 0x03:
								if (rs2 != 0x00) return {};
								return {VMV_V_I, rd, 0, 0, sign_extend(rs1, 5), rd};
							case 0x04:
								if (rs2 != 0x00) return {};
								return {VMV_V_X, rd, rs1, 0, 0, rd};
							case 0x05:
								if (rs2 != 0x00) return {};
								return {VFMV_V_F, rd, rs1, 0, 0, rd};
						}
						return {};
					case 0x25:
						if (f3 == 0x02) return {VMUL_VV, rd, rs2, rs1, 0, rd};
						return {};
					case 0x27:
						if (f3 == 0x03 && rs1 == 0x00) return {VMV1R_V, rd, rs2, 0, 0};
						return {};
					case 0x2C:
						if (f3 == 0x01) return {VFMACC_VV, rd, rd, rs1, 0, rs2};
						if (f3 == 0x05) return {VFMACC_VF, rd, rd, rs1, 0, rs2};
						return {};
					case 0x2D:
						if (f3 == 0x02) return {VMACC_VV, rd, rd, rs1, 0, rs2};
						if (f3 == 0x06) return {VMACC_VX, rd, rd, rs1, 0, rs2};
						return {};
				}
				return {};
			}
			case 0x63: {
				int32_t imm = sign_extend(
					((instruction >> 7)  & 0x1e)  |
					((instruction >> 20) & 0x7e0) |
					((instruction << 4)  & 0x800) |
					((instruction >> 19) & 0x1000), 13);
				switch(f3) {
					case 0x00: return {BEQ,  0, rs1, rs2, imm};
					case 0x01: return {BNE,  0, rs1, rs2, imm};
					case 0x04: return {BLT,  0, rs1, rs2, imm};
					case 0x05: return {BGE,  0, rs1, rs2, imm};
					case 0x06: return {BLTU, 0, rs1, rs2, imm};
					case 0x07: return {BGEU, 0, rs1, rs2, imm};
				}
				return {};
			}
			case 0x6F: {
				int32_t imm = sign_extend(
					((instruction >> 21) & 0x3FF) << 1  |
					((instruction >> 20) & 0x1)   << 11 |
					((instruction >> 12) & 0xFF)  << 12 |
					((instruction >> 31) & 0x1)   << 20, 21);
				return {JAL, rd, 0, 0, imm};
			}
			case 0x67: {
				int32_t imm = sign_extend(immu, 12);
				if (f3 != 0x00) return {};
				return {JALR, rd, rs1, 0, imm};
			}
			case 0x37:
				return {LUI, rd, 0, 0, (int32_t)(instruction & 0xFFFFF000)};
			case 0x17:
				return {AUIPC, rd, 0, 0, (int32_t)(instruction & 0xFFFFF000)};
			case 0x73:
				// if (instruction == 0x00000073) return {ECALL, 0, 0, 0, 0};
				// if (instruction >> 20 == 0x001) return {EBREAK, 0, 0, 0, 0};
				return {};
		}

		return {};
	}

private:
	static bool valid_rm(uint8_t rm) {
		return rm <= 4 || rm == 7;
	}

	static bool is_unit_stride_vector_mem(uint32_t instruction) {
		uint8_t lumop = (instruction >> 20) & 0x1F;
		uint8_t vm = (instruction >> 25) & 0x01;
		uint8_t mop = (instruction >> 26) & 0x03;
		uint8_t mew = (instruction >> 28) & 0x01;
		uint8_t nf = (instruction >> 29) & 0x07;
		return lumop == 0 && vm == 1 && mop == 0 && mew == 0 && nf == 0;
	}

	static uint8_t sew_from_vtype(uint32_t zimm) {
		if ((zimm & ~0xFFU) != 0) return 0; // reserved vtype bits must be zero
		if ((zimm & 0x7) != 0) return 0; // LMUL must be m1

		switch ((zimm >> 3) & 0x7) {
			case 0: return 8;
			case 1: return 16;
			case 2: return 32;
			default: return 0;
		}
	}
};
