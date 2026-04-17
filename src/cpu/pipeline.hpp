#pragma once
#include "instruction.hpp"

struct FetchEntry {
	uint32_t pc = 0;
	uint32_t instr = 0;
};

struct DecodeEntry {
	uint32_t pc = 0;
	Instruction instr;
	bool pred_taken = false;
    uint32_t pred_target = 0;
};

struct ExecEntry {
	Op op = INVALID;

	uint32_t value = 0, addr = 0;

	uint32_t target = 0;
	bool jumped = false, should_halt = false;

	uint32_t tag = -1U;
};

struct RSEntry {
	bool busy = false;

	Op op = INVALID;

	uint32_t Vj = 0, Vk = 0;
	uint32_t Qj = -1U, Qk = -1U;
	
	uint32_t pc = 0;
	int32_t imm = 0;

	uint32_t tag = -1U;
};
