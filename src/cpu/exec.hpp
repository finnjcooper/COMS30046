#pragma once
#include <optional>
#include "pipeline.hpp"

class ExecUnit {
public:
	virtual ~ExecUnit() = default;
	virtual optional<ExecEntry> step() = 0;
	
	void start(const RSEntry &entry) {
		current = entry;
		cycles_remaining = cycles;
		busy_ = true;
	}
	
	bool busy() const { return busy_; }
	void flush(uint32_t tag) { if (busy_ && current.tag > tag) busy_ = false; }

protected:
	RSEntry current;
	size_t cycles = 1UL;
	size_t cycles_remaining;
	bool busy_ = false;

	virtual uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) = 0;
};
