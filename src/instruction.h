#pragma once
#include <cstdint>

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
	ECALL, EBREAK,
	// rv32m
	MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU
};

bool isALU(Op op) { return op >= ADD && op <= AND || op >= MUL && op <= REMU; };
bool isALUI(Op op) { return op >= ADDI && op <= SRAI; };
bool isLoad(Op op) { return op >= LB && op <= LHU; };
bool isStore(Op op) { return op >= SB && op <= SW; };
bool isBranch(Op op) { return op >= BEQ && op <= BGEU; };
bool isJAL(Op op) { return op == JAL || op == JALR; };
bool isUI(Op op) { return op == LUI || op == AUIPC; };

bool writesRegister(Op op) { return isALU(op) || isALUI(op) || isLoad(op) || isUI(op) || isJAL(op); };

struct Instruction {
	Op op = INVALID;
	uint8_t rd, rs1, rs2;
	int32_t imm;
};


struct Program {
	vector<uint8_t> instrs;
	uint32_t entryPoint;
};
