#pragma once
#include <string>
#include "isa.hpp"
#include "trace.hpp"

class RegisterFile {
public:
	RegisterFile(uint8_t num_regs, CommitLog &log) : num_regs(num_regs), log(log), regs(num_regs, Value::scalar(0U)) {}
	virtual Value read(uint8_t index) const = 0;
	virtual void write(uint8_t index, Value value) = 0;
	virtual string name(uint8_t index) const = 0;
	virtual void clear() {
		fill(regs.begin(), regs.end(), Value::scalar(0U));
	}
	vector<Value> snapshot() const {
		return regs;
	}
protected:
	uint8_t num_regs;
	CommitLog &log;
	vector<Value> regs;
};

class IntegerRegisterFile : public RegisterFile {
public:
	IntegerRegisterFile(uint8_t num_regs, CommitLog &log) : RegisterFile(num_regs, log) {}

	Value read(uint8_t index) const override {
		if (index >= num_regs) throw out_of_range("Register index out of range");
		return regs[index];
	}

	void write(uint8_t index, Value value) override {
		if (index >= num_regs) throw out_of_range("Register index out of range");
		else if (index == 0) return; // discard writes to x0

		uint32_t old = regs[index].as_scalar();
		uint32_t next = value.as_scalar();
		log.record_reg_write(index, old, next);
		regs[index] = Value::scalar(next);
	}

	string name(uint8_t index) const override {
		if (index >= num_regs) throw out_of_range("Register index out of range");
		if (index >= NUM_NAMES) return "x" + to_string(index);
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

	Value read(uint8_t index) const override {
		if (index >= num_regs) throw out_of_range("Float register index out of range");
		return regs[index];
	}

	void write(uint8_t index, Value value) override {
		if (index >= num_regs) throw out_of_range("Float register index out of range");

		uint32_t old = regs[index].as_scalar();
		uint32_t next = value.as_scalar();
		log.record_reg_write(index, old, next);
		regs[index] = Value::scalar(next);
	}

	string name(uint8_t index) const override {
		if (index >= num_regs) throw out_of_range("Float register index out of range");
		return "f" + to_string(index);
	}
};

class VectorRegisterFile : public RegisterFile {
public:
	VectorRegisterFile(uint8_t num_regs, CommitLog &log) : RegisterFile(num_regs, log) {
		for (auto &reg : regs) reg = Value::vector_zero();
	}

	void clear() override {
		fill(regs.begin(), regs.end(), Value::vector_zero());
	}

	Value read(uint8_t index) const override {
		if (index >= num_regs) throw out_of_range("Vector register index out of range");
		return regs[index];
	}

	void write(uint8_t index, Value value) override {
		if (index >= num_regs) throw out_of_range("Vector register index out of range");

		log.record_reg_write(index, regs[index].as_scalar(), value.as_scalar());
		value.vector_value = true;
		regs[index] = value;
	}

	string name(uint8_t index) const override {
		if (index >= num_regs) throw out_of_range("Vector register index out of range");
		return "v" + to_string(index);
	}
};
