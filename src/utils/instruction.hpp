#pragma once
#include <cstdint>
#include <vector>

using namespace std;

inline constexpr uint8_t XLEN = 32;
inline constexpr uint8_t WORD_BYTES = XLEN / 8;

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
	ECALL, EBREAK,
	// rv32m
	MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU
};

inline bool isALU(Op op) { return op >= ADD && op <= AND; }
inline bool isALUI(Op op) { return op >= ADDI && op <= SRAI; }
inline bool isMUL(Op op) { return op >= MUL && op <= REMU; }
inline bool isLoad(Op op) { return op >= LB && op <= LHU; }
inline bool isStore(Op op) { return op >= SB && op <= SW; }
inline bool isBranch(Op op) { return op >= BEQ && op <= BGEU; }
inline bool isJAL(Op op) { return op == JAL || op == JALR; }
inline bool isUI(Op op) { return op == LUI || op == AUIPC; }

inline bool writesRegister(Op op) { return isALU(op) || isALUI(op) || isMUL(op) || isLoad(op) || isUI(op) || isJAL(op); }

struct Instruction {
	Op op = INVALID;
	uint8_t rd, rs1, rs2;
	int32_t imm;
};


inline int32_t signExtend(uint32_t value, int bits) {
	int32_t shift = 32 - bits;
	return (int32_t)(value << shift) >> shift;
}
