#pragma once
#include "trace.hpp"
#include "isa.hpp"

class RegisterFile {
public:
	RegisterFile(uint8_t num_regs, CommitLog &log) : num_regs(num_regs), log(log), regs(num_regs, 0U) {}
	virtual uint32_t read(uint8_t index) const = 0;
	virtual void write(uint8_t index, uint32_t value) = 0;
	virtual const char* name(uint8_t index) const = 0;
protected:
	uint8_t num_regs;
	CommitLog &log;
	vector<uint32_t> regs;
};

class IntegerRegisterFile : public RegisterFile {
public:
	IntegerRegisterFile(uint8_t num_regs, CommitLog &log) : RegisterFile(num_regs, log) {}

	uint32_t read(uint8_t index) const {
		if (index >= num_regs) throw out_of_range("Register index out of range");
		return regs[index];
	}

	void write(uint8_t index, uint32_t value) {
		if (index >= num_regs) throw out_of_range("Register index out of range");
		else if (index == 0) return; // discard writes to x0

		uint32_t old = regs[index];
		log.record_reg_write(index, old, value);
		regs[index] = value;
	}

	const char* name(uint8_t index) const {
		if (index >= num_regs) throw out_of_range("Register index out of range");
		if (index >= NUM_NAMES) return ("x" + to_string(index)).c_str();
		return NAMES[index];
	}

private:
	static constexpr uint8_t NUM_NAMES = 32;
	static constexpr const char* NAMES[NUM_NAMES] = {
		"zero","ra","sp","gp","tp","t0","t1","t2",
		"s0","s1","a0","a1","a2","a3","a4","a5",
		"a6","a7","s2","s3","s4","s5","s6","s7",
		"s8","s9","s10","s11","t3","t4","t5","t6"
	};
};

class FloatRegisterFile : public RegisterFile {
public:
	FloatRegisterFile(uint8_t num_regs, CommitLog &log) : RegisterFile(num_regs, log) {}

	uint32_t read(uint8_t index) const {
		if (index >= num_regs) throw out_of_range("Float register index out of range");
		return regs[index];
	}

	void write(uint8_t index, uint32_t value) {
		if (index >= num_regs) throw out_of_range("Float register index out of range");

		uint32_t old = regs[index];
		log.record_reg_write(index, old, value);
		regs[index] = value;
	}

	const char* name(uint8_t index) const {
		if (index >= num_regs) throw out_of_range("Float register index out of range");
		return ("f" + to_string(index)).c_str();
	}
};
