#pragma once
#include "instruction.hpp"

struct ROBEntry {
	bool busy, ready;
	Op op;

	uint8_t dest_reg;
	uint32_t value, addr;

	uint32_t tag;
};

class ReOrderBuffer {
public:
	ReOrderBuffer(uint32_t size) : entries(size), head(0), tail(0), count(0) {}

	uint32_t allocate(Op op, uint8_t dest_reg) {
		if (count == entries.size()) return -1U;
		uint32_t tag = tail;
		entries[tag] = {true, false, op, dest_reg, 0, 0, tag};
		tail = (tail + 1) % entries.size();
		count++;
		return tag;
	}

	void set(uint32_t tag, uint32_t value, uint32_t addr = 0) {
		entries[tag].value = value;
		entries[tag].addr = addr;
		entries[tag].ready = true;
	}

	bool canCommit() const {
		return count > 0 && entries[head].ready;
	}

	ROBEntry& get(uint32_t tag) {
		return entries[tag];
	}

	ROBEntry& front() {
		return entries[head];
	}

	void pop() {
		entries[head] = {};
		head = (head + 1) % entries.size();
		count--;
	}

	void flush() {
		for (auto &entry : entries) entry = {};
		head = tail = count = 0;
	}

private:
	vector<ROBEntry> entries;
	uint32_t head, tail, count;
};
