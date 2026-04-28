#pragma once
#include <deque>
#include <algorithm>
#include "decode.hpp"
#include "exec.hpp"

struct ROBEntry {
	bool ready = false, jumped = false;
	bool ctrl_handled = false;
	
	bool pred_taken = false;
	uint32_t pred_target = 0;

	Op op = INVALID;

	uint8_t rd = 0;
	Value value = 0;
	uint32_t target = 0;
	uint32_t pc = 0;
	uint8_t vl = 0, sew = 0;

	uint32_t tag = -1U;
};

class ReOrderBuffer {
public:
	ReOrderBuffer(uint32_t size) : max_size(size) {}

	deque<ROBEntry>& get_entries() { return entries; }
	const deque<ROBEntry>& get_entries() const { return entries; }
	bool empty() const { return entries.empty(); }

	uint32_t allocate(DecodeEntry decode) {
		if (entries.size() == max_size) return -1U;
		uint32_t tag = next_tag++;
		ROBEntry entry;
		entry.pred_taken = decode.pred_taken;
		entry.pred_target = decode.pred_target;
		entry.op = decode.instr.op;
		entry.rd = decode.instr.rd;
		entry.pc = decode.pc;
		entry.tag = tag;
		entries.push_back(entry);
		return tag;
	}

	bool can_commit() const { return !entries.empty() && entries.front().ready; }

	void update(ExecEntry exec) {
		for (auto& entry : entries) {
			if (entry.tag == exec.tag) {
				entry.value = exec.value;
				entry.target = exec.target;
				entry.jumped = exec.jumped;
				entry.vl = exec.vl;
				entry.sew = exec.sew;
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
