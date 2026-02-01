#pragma once
#include <cstdint>
#include <vector>

using namespace std;

struct RegWrite {
	uint8_t reg;
	uint32_t oldVal;
	uint32_t newVal;
};

struct MemWrite {
	uint32_t addr;
	uint32_t oldVal;
	uint32_t newVal;
	uint8_t size;
};

struct CommitLog {
	vector<RegWrite> regWrites;
	vector<MemWrite> memWrites;

	void recordRegWrite(uint8_t reg, uint32_t oldVal, uint32_t newVal) {
		regWrites.push_back({reg, oldVal, newVal});
	}

	void recordMemWrite(uint32_t addr, uint32_t oldVal, uint32_t newVal, uint8_t size) {
		memWrites.push_back({addr, oldVal, newVal, size});
	}

	void clear() {
		regWrites.clear();
		memWrites.clear();
	}
};