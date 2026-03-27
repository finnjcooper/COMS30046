#pragma once
#include "instruction.hpp"
#include <deque>
#include <algorithm>
#include <stdexcept>

struct ROBEntry {
	bool ready, jumped, should_halt;
	Op op;

	uint8_t rd;
	uint32_t value, addr;

	uint32_t tag;
};

class ReOrderBuffer {
public:
	ReOrderBuffer(uint32_t size) : max_size(size) {}

	std::deque<ROBEntry>& getEntries() { return entries; }

	uint32_t allocate(Op op, uint8_t rd) {
		if (entries.size() == max_size) return -1U;
		uint32_t tag = next_tag++;
		entries.push_back({false, false, false, op, rd, 0, 0, tag});
		return tag;
	}

	bool canCommit() const { return !entries.empty() && entries.front().ready; }

	void set(uint32_t tag, uint32_t value, uint32_t addr = 0, bool jumped = false, bool should_halt = false) {
		for (auto& entry : entries) {
			if (entry.tag == tag) {
				entry.value = value;
				entry.addr = addr;
				entry.ready = true;
				entry.jumped = jumped;
				entry.should_halt = should_halt;
				break;
			}
		}
	}

	ROBEntry& get(uint32_t tag) {
		for (auto& entry : entries) {
			if (entry.tag == tag) return entry;
		}

		throw out_of_range("ROB tag not found");
	}

	ROBEntry& front() { return entries.front(); }

	void pop() {
		if (!entries.empty()) entries.pop_front();
	}

	void flush(uint32_t tag) {
		entries.erase(
			remove_if(entries.begin(), entries.end(), [tag](const ROBEntry& entry) { return entry.tag > tag; }),
			entries.end()
		);
	}

private:
	deque<ROBEntry> entries;
	uint32_t max_size;
	uint32_t next_tag = 0;
};
