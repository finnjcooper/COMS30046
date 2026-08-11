#pragma once
#include "rob.hpp"

struct RATSnapshot {
	vector<uint32_t> table;
};

class RegisterAliasTable {
public:
	RegisterAliasTable(size_t num_regs) : table(num_regs, -1U) {}

	void clear() {
		fill(table.begin(), table.end(), -1U);
	}

	RATSnapshot snapshot() const {
		return { table };
	}

	uint32_t get(uint32_t reg) const {
		return table[reg];
	}

	void set(uint32_t reg, uint32_t tag) {
		table[reg] = tag;
	}

	void rebuild(ReOrderBuffer &rob, RegType type) {
		for (auto &entry : table) entry = -1U;
		for (auto &entry : rob.get_entries())
			if (dst_type(entry.op) == type && !(type == RegType::INT && entry.rd == 0))
				table[entry.rd] = entry.tag;
	}

private:
	vector<uint32_t> table;
};
