#include "instruction.hpp"

struct ROBEntry {
	bool busy;
	Op op;

	uint32_t dest_reg;
	uint32_t value;
	bool ready;

	uint32_t store_addr;
	uint32_t store_value;

	uint32_t tag;
};

class ROB {
public:
	ROB(uint32_t size) : entries(size), head(0), tail(0), count(0) {}

	uint32_t allocate(Op op, uint32_t dest_reg) {
		if (count == entries.size()) return -1;
		uint32_t tag = tail;
		entries[tag] = {true, op, dest_reg, 0, false, 0, 0, tail};
		tail = (tail + 1) % entries.size();
		count++;
		return tag;
	}

	void write(uint32_t tag, uint32_t value) {
		entries[tag].value = value;
		entries[tag].ready = true;
	}

	bool canCommit() const {
		return count > 0 && entries[head].ready;
	}

	ROBEntry& front() {
		return entries[head];
	}

	void pop() {
		entries[head] = {};
		head = (head + 1) % entries.size();
		count--;
	}

private:
	vector<ROBEntry> entries;
	uint32_t head, tail, count;
};

struct RSEntry {
	Op op;

	uint32_t Qj = 0, Qk = 0;
	uint32_t Vj = 0, Vk = 0;

	uint32_t dest = -1U;
	bool busy = false;
};
