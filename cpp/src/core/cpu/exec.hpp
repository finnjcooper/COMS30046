#pragma once
#include <optional>
#include "helpers.hpp"

struct ExecEntry {
	Op op = INVALID;

	Value value = 0;

	uint32_t target = 0;
	bool jumped = false;

	uint32_t tag = -1U;
	uint8_t vl = 0;
	uint8_t sew = 0;
};

struct RSEntry {
	bool busy = false;

	Op op = INVALID;

	Value Vj = 0, Vk = 0, Vl = 0;
	uint32_t Qj = -1U, Qk = -1U, Ql = -1U, Qv = -1U;
	
	uint32_t pc = 0;
	int32_t imm = 0;
	uint8_t rm = 0;
	uint8_t vl = 0;
	uint8_t sew = 0;

	uint32_t tag = -1U;
};

class ExecUnit {
public:
	virtual ~ExecUnit() = default;
	virtual optional<ExecEntry> step() = 0;

	void clear() { busy_ = false; }

	void start(const RSEntry &entry) {
		current = entry;
		cycles_remaining = latency(current);
		busy_ = true;
	}
	
	bool busy() const { return busy_; }
	void flush(uint32_t tag) { if (busy_ && current.tag > tag) busy_ = false; }

protected:
	RSEntry current;
	size_t cycles_remaining;
	bool busy_ = false;

	virtual Value exec(Op op, Value operand1, Value operand2, Value operand3 = Value::scalar(0)) = 0;
	virtual size_t latency(const RSEntry &entry) const { return cycles(entry.op); }
};
