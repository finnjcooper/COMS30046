#pragma once
#include "instruction.hpp"

struct FetchEntry {
	uint32_t pc = 0;
	uint32_t instr = 0;
};

struct DecodeEntry {
	uint32_t pc = 0;
	Instruction instr;
};

struct ExecEntry {
	Op op;

	uint32_t value, addr;

	uint32_t target = 0;
	bool jumped = false, should_halt = false;

	uint32_t tag = -1U;
};

struct RSEntry {
	bool busy = false;

	Op op;

	uint32_t Vj = -1U, Vk = -1U;
	uint32_t Qj = -1U, Qk = -1U;
	
	uint32_t pc;
	int32_t imm;

	uint32_t tag = -1U;
};
