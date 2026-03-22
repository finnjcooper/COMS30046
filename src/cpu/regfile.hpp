#pragma once
#include <stdexcept>
#include "trace.hpp"
#include "instruction.hpp"

class RegisterFile {
public:
	RegisterFile(uint8_t num_regs, CommitLog &log) : num_regs(num_regs), log(log) {}

	uint32_t read(uint8_t index) const {
		if (index < 0 || index >= num_regs) throw out_of_range("Register index out of range");
		return regs[index];
	}

	void write(uint8_t index, uint32_t value) {
		if (index < 0 || index >= num_regs) throw out_of_range("Register index out of range");
		else if (index == 0) return; // discard writes to x0

		uint32_t old = regs[index];
		if (old != value) log.recordRegWrite(index, old, value);
		regs[index] = value;
	}

	const char* name(uint8_t index) const {
		if (index < 0 || index >= num_regs) throw out_of_range("Register index out of range");
		return NAMES[index];
	}

private:
	uint8_t num_regs;
	CommitLog &log;

	vector<uint32_t> regs = vector<uint32_t>(num_regs, 0);

	static constexpr const char* NAMES[NUM_REGISTERS] = {
		"zero","ra","sp","gp","tp","t0","t1","t2",
		"s0","s1","a0","a1","a2","a3","a4","a5",
		"a6","a7","s2","s3","s4","s5","s6","s7",
		"s8","s9","s10","s11","t3","t4","t5","t6"
	};
};
