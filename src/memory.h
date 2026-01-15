#pragma once
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

struct MemCell {
	uint8_t prev = 0;
	uint8_t curr = 0;
};

class Memory {
public:
	~Memory() = default;
	Memory(size_t size) { mem = vector<MemCell>(size); }
	Memory(vector<uint8_t> init, size_t size) {
		mem.resize(size);
		for (size_t i = 0; i < init.size() && i < size; i++) {
			mem[i].curr = init[i];
			mem[i].prev = init[i];
		}
	}

	void storeb(uint32_t addr, uint8_t value) {
		mem[addr].curr = value;
	}

	void storeh(uint32_t addr, uint16_t value) {
		mem[addr].curr = value & 0xFF;
		mem[addr + 1].curr = (value >> 8) & 0xFF;
	}
	
	void storew(uint32_t addr, uint32_t value) {
		mem[addr].curr = value & 0xFF;
		mem[addr + 1].curr = (value >> 8) & 0xFF;
		mem[addr + 2].curr = (value >> 16) & 0xFF;
		mem[addr + 3].curr = (value >> 24) & 0xFF;
	}

	uint8_t loadb(uint32_t addr) const { return mem[addr].curr;}
	uint16_t loadh(uint32_t addr) const { return (mem[addr].curr) | (mem[addr + 1].curr << 8); }
	uint32_t loadw(uint32_t addr) const { return (mem[addr].curr) | (mem[addr + 1].curr << 8) | (mem[addr + 2].curr << 16) | (mem[addr + 3].curr << 24); }
	uint32_t loadwPrev(uint32_t addr) const { return (mem[addr].prev) | (mem[addr + 1].prev << 8) | (mem[addr + 2].prev << 16) | (mem[addr + 3].prev << 24); }
	size_t size() const { return mem.size(); }
	
	void print8() {
		for (size_t addr = 0; addr < mem.size(); addr++) {
			cout << "0x" << setw(2) << setfill('0') << hex << (int)addr << ": 0x" << setw(2) << setfill('0') << loadb(addr) << dec << endl;
		}
	}
	
	void print32() {
		for (size_t addr = 0; addr < mem.size(); addr += 4) {
			if (loadw(addr) == 0) continue;
			cout << "0x" << setw(8) << setfill('0') << hex << (int)addr << ": 0x" << setw(8) << setfill('0') << loadw(addr) << dec << endl;
		}
	}

	void commit() {
		for (auto& cell : mem) {
			cell.prev = cell.curr;
		}
	}

private:
	vector<MemCell> mem;
};
