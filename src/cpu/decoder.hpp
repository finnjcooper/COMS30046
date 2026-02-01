#pragma once
#include "instruction.hpp"
#include <iomanip>

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
			case 0x33:
				if (f7 == 0x20) {
					if (f3 == 0x00) return {SUB, rd, rs1, rs2, 0};
					if (f3 == 0x05) return {SRA, rd, rs1, rs2, 0};
				} else if (f7 == 0x00) {
					if (f3 == 0x00) return {ADD, rd, rs1, rs2, 0};
					if (f3 == 0x01) return {SLL, rd, rs1, rs2, 0};
					if (f3 == 0x02) return {SLT, rd, rs1, rs2, 0};
					if (f3 == 0x03) return {SLTU, rd, rs1, rs2, 0};
					if (f3 == 0x04) return {XOR, rd, rs1, rs2, 0};
					if (f3 == 0x05) return {SRL, rd, rs1, rs2, 0};
					if (f3 == 0x06) return {OR, rd, rs1, rs2, 0};
					if (f3 == 0x07) return {AND, rd, rs1, rs2, 0};
				} else if (f7 == 0x01) {
					if (f3 == 0x00) return {MUL, rd, rs1, rs2, 0};
					if (f3 == 0x01) return {MULH, rd, rs1, rs2, 0};
					if (f3 == 0x02) return {MULHSU, rd, rs1, rs2, 0};
					if (f3 == 0x03) return {MULHU, rd, rs1, rs2, 0};
					if (f3 == 0x04) return {DIV, rd, rs1, rs2, 0};
					if (f3 == 0x05) return {DIVU, rd, rs1, rs2, 0};
					if (f3 == 0x06) return {REM, rd, rs1, rs2, 0};
					if (f3 == 0x07) return {REMU, rd, rs1, rs2, 0};
				}
				cerr << "Unknown R-type instruction." << endl;
				break;
			case 0x13: {
				int32_t imm = sign_extend(immu, 12);
				if (f3 == 0x00) return {ADDI, rd, rs1, 0, imm};
				if (f3 == 0x01) return {SLLI, rd, rs1, 0, rs2};
				if (f3 == 0x02) return {SLTI, rd, rs1, 0, imm};
				if (f3 == 0x03) return {SLTIU, rd, rs1, 0, imm};
				if (f3 == 0x04) return {XORI, rd, rs1, 0, imm};
				if (f3 == 0x05) return (f7 == 0x20)
					? Instruction {SRAI, rd, rs1, 0, rs2}
					: Instruction {SRLI, rd, rs1, 0, rs2};
				if (f3 == 0x06) return {ORI, rd, rs1, 0, imm};
				if (f3 == 0x07) return {ANDI, rd, rs1, 0, imm};
				cerr << "Unknown I-type instruction." << endl;
				break;
			}
			case 0x03: {
				int32_t imm = sign_extend(immu, 12);
				if (f3 == 0x00) return {LB,  rd, rs1, 0, imm};
				if (f3 == 0x01) return {LH,  rd, rs1, 0, imm};
				if (f3 == 0x02) return {LW,  rd, rs1, 0, imm};
				if (f3 == 0x04) return {LBU, rd, rs1, 0, imm};
				if (f3 == 0x05) return {LHU, rd, rs1, 0, imm};
				cerr << "Unknown load instruction." << endl;
				break;
			}
			case 0x23: {
				int32_t imm = sign_extend((f7 << 5) | rd, 12);
				if (f3 == 0x00) return {SB, 0, rs1, rs2, imm};
				if (f3 == 0x01) return {SH, 0, rs1, rs2, imm};
				if (f3 == 0x02) return {SW, 0, rs1, rs2, imm};
				cerr << "Unknown store instruction." << endl;
				break;
			}
			case 0x63: {
				int32_t imm = sign_extend(
					((instruction >> 7)  & 0x1e)  |
					((instruction >> 20) & 0x7e0) |
					((instruction << 4)  & 0x800) |
					((instruction >> 19) & 0x1000), 13);
				if (f3 == 0x00) return {BEQ,  0, rs1, rs2, imm};
				if (f3 == 0x01) return {BNE,  0, rs1, rs2, imm};
				if (f3 == 0x04) return {BLT,  0, rs1, rs2, imm};
				if (f3 == 0x05) return {BGE,  0, rs1, rs2, imm};
				if (f3 == 0x06) return {BLTU, 0, rs1, rs2, imm};
				if (f3 == 0x07) return {BGEU, 0, rs1, rs2, imm};
				cerr << "Unknown branch instruction." << endl;
				break;
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
				return {JALR, rd, rs1, 0, imm};
			}
			case 0x37:
				return {LUI, rd, 0, 0, (int32_t)(instruction & 0xFFFFF000)};
			case 0x17:
				return {AUIPC, rd, 0, 0, (int32_t)(instruction & 0xFFFFF000)};
			case 0x73:
				if (instruction >> 20 == 0x000) return {ECALL, 0, 0, 0, 0};
				if (instruction >> 20 == 0x001) return {EBREAK, 0, 0, 0, 0};
				cerr << "Unknown system instruction." << endl;
				break;
			default:
				cerr << "Unknown opcode: 0x" << setw(2) << setfill('0') << hex << (int)op << dec << endl;
				break;
		}

		return Instruction();
	}
};
