#pragma once
#include <cstdint>
#include <vector>

using namespace std;

struct RegWrite {
	uint8_t reg;
	uint32_t old_val;
	uint32_t new_val;
};

struct MemWrite {
	uint32_t addr;
	uint32_t old_val;
	uint32_t new_val;
	uint8_t size;
};

struct CommitLog {
	vector<RegWrite> reg_writes;
	vector<MemWrite> mem_writes;

	void record_reg_write(uint8_t reg, uint32_t old_val, uint32_t new_val) {
		reg_writes.push_back({reg, old_val, new_val});
	}

	void record_mem_write(uint32_t addr, uint32_t old_val, uint32_t new_val, uint8_t size) {
		mem_writes.push_back({addr, old_val, new_val, size});
	}

	void clear() {
		reg_writes.clear();
		mem_writes.clear();
	}
};
