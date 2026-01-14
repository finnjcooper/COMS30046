#include "decoder.h"

Instruction Decoder::decode(uint32_t instruction) {
	uint8_t op = instruction & 0x7F;
	uint8_t f3 = (instruction >> 12) & 0x07;
	uint8_t f7 = (instruction >> 25) & 0x7F;
	uint8_t rd = (instruction >> 7) & 0x1F;
	uint8_t rs1 = (instruction >> 15) & 0x1F;
	uint8_t rs2 = (instruction >> 20) & 0x1F;
	
	switch (op) {
		case 0x33:
			if (f7 == 0x20) {
				if (f3 == 0x00) return {Opcode::SUB, rd, rs1, rs2};
				if (f3 == 0x05) return {Opcode::SRA, rd, rs1, rs2};
			} else {
				if (f3 == 0x00) return {Opcode::ADD, rd, rs1, rs2};
				if (f3 == 0x01) return {Opcode::SLL, rd, rs1, rs2};
				if (f3 == 0x02) return {Opcode::SLT, rd, rs1, rs2};
				if (f3 == 0x03) return {Opcode::SLTU, rd, rs1, rs2};
				if (f3 == 0x04) return {Opcode::XOR, rd, rs1, rs2};
				if (f3 == 0x05) return {Opcode::SRL, rd, rs1, rs2};
				if (f3 == 0x06) return {Opcode::OR, rd, rs1, rs2};
				if (f3 == 0x07) return {Opcode::AND, rd, rs1, rs2};
			} break;
		case 0x13: {
			int32_t imm = sign_extend(instruction >> 20, 12);
			if (f3 == 0x00) return {Opcode::ADDI, rd, rs1, imm};
			if (f3 == 0x01) return {Opcode::SLLI, rd, rs1, rs2};
			if (f3 == 0x02) return {Opcode::SLTI, rd, rs1, imm};
			if (f3 == 0x03) return {Opcode::SLTIU, rd, rs1, imm};
			if (f3 == 0x04) return {Opcode::XORI, rd, rs1, imm};
			if (f3 == 0x05) return (f7 == 0x20)
				? Instruction {Opcode::SRAI, rd, rs1, rs2}
				: Instruction {Opcode::SRLI, rd, rs1, rs2};
			if (f3 == 0x06) return {Opcode::ORI, rd, rs1, imm};
			if (f3 == 0x07) return {Opcode::ANDI, rd, rs1, imm};
			break;
		}
		case 0x03: {
			int32_t imm = sign_extend(instruction >> 20, 12);
			if (f3 == 0x00) return {Opcode::LB,  rd, rs1, imm};
			if (f3 == 0x01) return {Opcode::LH,  rd, rs1, imm};
			if (f3 == 0x02) return {Opcode::LW,  rd, rs1, imm};
			if (f3 == 0x04) return {Opcode::LBU, rd, rs1, imm};
			if (f3 == 0x05) return {Opcode::LHU, rd, rs1, imm};
			break;
		}
		case 0x23: {
			int32_t imm = sign_extend(((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F), 12);
			if (f3 == 0x00) return {Opcode::SB, rs1, rs2, imm};
			if (f3 == 0x01) return {Opcode::SH, rs1, rs2, imm};
			if (f3 == 0x02) return {Opcode::SW, rs1, rs2, imm};
			break;
		}
		case 0x63: {
			int32_t imm = sign_extend(
				((instruction >> 7)  & 0x1e)  |
				((instruction >> 20) & 0x7e0) |
				((instruction << 4)  & 0x800) |
				((instruction >> 19) & 0x1000), 13);
			if (f3 == 0x00) return {Opcode::BEQ,  rs1, rs2, imm};
			if (f3 == 0x01) return {Opcode::BNE,  rs1, rs2, imm};
			if (f3 == 0x04) return {Opcode::BLT,  rs1, rs2, imm};
			if (f3 == 0x05) return {Opcode::BGE,  rs1, rs2, imm};
			if (f3 == 0x06) return {Opcode::BLTU, rs1, rs2, imm};
			if (f3 == 0x07) return {Opcode::BGEU, rs1, rs2, imm};
			break;
		}
		case 0x6F: {
			int32_t imm = sign_extend(
				((instruction >> 21) & 0x3FF) << 1  |
				((instruction >> 20) & 0x1)   << 11 |
				((instruction >> 12) & 0xFF)  << 12 |
				((instruction >> 31) & 0x1)   << 20, 21);
			return {Opcode::JAL, rd, 0, imm};
		}
		case 0x67: {
			int32_t imm = sign_extend(instruction >> 20, 12);
			return {Opcode::JALR, rd, rs1, imm};
		}
		case 0x37:
			return {Opcode::LUI, rd, 0, (int32_t)(instruction & 0xFFFFF000)};
		case 0x17:
			return {Opcode::AUIPC, rd, 0, (int32_t)(instruction & 0xFFFFF000)};
		case 0x73:
			if (instruction >> 20 == 0x000) return {Opcode::ECALL, 0, 0, 0};
			if (instruction >> 20 == 0x001) return {Opcode::EBREAK, 0, 0, 0};
			break;
		default:
			return {Opcode::INVALID, 0, 0, 0};
	}
}