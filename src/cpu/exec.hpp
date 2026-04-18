#pragma once
#include <optional>
#include "helpers.hpp"

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

	uint32_t Vj = 0, Vk = 0, Vl = 0;
	uint32_t Qj = -1U, Qk = -1U, Ql = -1U;
	
	uint32_t pc = 0;
	int32_t imm = 0;

	uint32_t tag = -1U;
};

class ExecUnit {
public:
	virtual ~ExecUnit() = default;
	virtual optional<ExecEntry> step() = 0;
	
	void start(const RSEntry &entry) {
		current = entry;
		cycles_remaining = cycles(current.op);
		busy_ = true;
	}
	
	bool busy() const { return busy_; }
	void flush(uint32_t tag) { if (busy_ && current.tag > tag) busy_ = false; }

protected:
	RSEntry current;
	size_t cycles_remaining;
	bool busy_ = false;

	virtual uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) = 0;
};
