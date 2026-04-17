#pragma once
#include "instruction.hpp"
#include <deque>
#include <algorithm>
#include <stdexcept>

struct ROBEntry {
	bool ready = false, jumped = false, should_halt = false;
	
	bool pred_taken = false;
	uint32_t pred_target = 0;

	Op op = INVALID;

	uint8_t rd = 0;
	uint32_t value = 0, addr = 0;
	uint32_t pc = 0;

	uint32_t tag = -1U;
};

class ReOrderBuffer {
public:
	ReOrderBuffer(uint32_t size) : max_size(size) {}

	const deque<ROBEntry>& get_entries() const { return entries; }

	uint32_t allocate(DecodeEntry decode) {
		if (entries.size() == max_size) return -1U;
		uint32_t tag = next_tag++;
		entries.push_back({false, false, false, decode.pred_taken, decode.pred_target, decode.instr.op, decode.instr.rd, 0, 0, decode.pc, tag});
		return tag;
	}

	bool can_commit() const { return !entries.empty() && entries.front().ready; }

	void update(ExecEntry exec) {
		for (auto& entry : entries) {
			if (entry.tag == exec.tag) {
				entry.value = exec.value;
				entry.addr = exec.addr;
				entry.jumped = exec.jumped;
				entry.should_halt = exec.should_halt;
				entry.ready = true;
				break;
			}
		}
	}

	ROBEntry& get(uint32_t tag) {
		for (auto& entry : entries)
			if (entry.tag == tag) return entry;

		throw out_of_range("ROB tag not found");
	}

	ROBEntry& front() { return entries.front(); }
	void pop() { if (!entries.empty()) entries.pop_front(); }

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
