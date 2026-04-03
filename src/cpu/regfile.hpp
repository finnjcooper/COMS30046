#pragma once
#include <stdexcept>
#include "trace.hpp"
#include "instruction.hpp"
#include "rob.hpp"

class RegisterFile {
public:
	RegisterFile(uint8_t num_regs, CommitLog &log) : num_regs(num_regs), log(log) {}

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
		return NAMES[index];
	}

private:
	uint8_t num_regs;
	CommitLog &log;

	vector<uint32_t> regs = vector<uint32_t>(num_regs, 0);

	static constexpr const char* NAMES[32] = {
		"zero","ra","sp","gp","tp","t0","t1","t2",
		"s0","s1","a0","a1","a2","a3","a4","a5",
		"a6","a7","s2","s3","s4","s5","s6","s7",
		"s8","s9","s10","s11","t3","t4","t5","t6"
	};
};

class RegisterAliasTable {
public:
	RegisterAliasTable(size_t num_regs) : table(num_regs, -1U) {}

	uint32_t get(uint32_t reg) const {
		return table[reg];
	}

	void set(uint32_t reg, uint32_t tag) {
		table[reg] = tag;
	}

	void rebuild(ReOrderBuffer &rob) {
		for (auto &entry : table) entry = -1U;
		for (auto &entry : rob.get_entries())
			if (writes_register(entry.op) && entry.rd != 0)
				table[entry.rd] = entry.tag;
	}

private:
	vector<uint32_t> table;
};
