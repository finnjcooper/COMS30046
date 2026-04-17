#pragma once
#include "rob.hpp"

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
