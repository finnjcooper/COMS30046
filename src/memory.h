#pragma once
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

class Memory {
public:
	Memory(size_t size) { mem = vector<uint8_t>(size, 0); }
	Memory(vector<uint8_t> init, size_t size) { mem = vector<uint8_t>(init.begin(), init.end()); mem.resize(size); }
	~Memory() = default;

	void storeb(uint32_t addr, uint8_t value) { mem[addr] = value; }

	void storeh(uint32_t addr, uint16_t value) {
		mem[addr]     = value & 0xFF;
		mem[addr + 1] = (value >> 8) & 0xFF;
	}
	
	void storew(uint32_t addr, uint32_t value) {
		mem[addr]     = value & 0xFF;
		mem[addr + 1] = (value >> 8) & 0xFF;
		mem[addr + 2] = (value >> 16) & 0xFF;
		mem[addr + 3] = (value >> 24) & 0xFF;
	}

	uint8_t loadb(uint32_t addr) { return mem[addr];}
	uint16_t loadh(uint32_t addr) { return (mem[addr]) | (mem[addr + 1] << 8); }
	uint32_t loadw(uint32_t addr) { return (mem[addr]) | (mem[addr + 1] << 8) | (mem[addr + 2] << 16) | (mem[addr + 3] << 24); }
	
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

private:
	vector<uint8_t> mem;
};
