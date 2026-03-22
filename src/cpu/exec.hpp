#pragma once
#include "instruction.hpp"
#include "pipeline.hpp"

class ExecUnit {
public:
	virtual void step() = 0;
	
	void start(ExecEntry idex) {
		current = idex;
		cycles_remaining = cycles;
		busy_ = true;
	}
	
	bool busy() const { return busy_; }
	bool done() const { return done_; }
	CommitEntry getResult() { done_ = false; return result; }
	void flush() { busy_ = false; done_ = false; }

protected:
	ExecEntry current;
	CommitEntry result;
	size_t cycles = 1UL;
	size_t cycles_remaining;
	bool busy_ = false;
	bool done_ = false;

	virtual uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) = 0;
};
