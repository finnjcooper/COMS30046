#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

class Memory {
public:
	~Memory() = default;
	Memory(size_t size) { data.resize(size, 0); }
	Memory(vector<uint8_t> init, size_t size) {
		data.resize(size, 0);
		for (size_t i = 0; i < init.size() && i < size; i++) data[i] = init[i];
	}

	void storeb(uint32_t addr, uint8_t value) {
		if (addr >= data.size()) throw out_of_range("Memory access out of range");
		data[addr] = value;
	}

	void storeh(uint32_t addr, uint16_t value) {
		if (addr + 1 >= data.size()) throw out_of_range("Memory access out of range");
		data[addr] = value & 0xFF;
		data[addr + 1] = (value >> 8) & 0xFF;
	}
	
	void storew(uint32_t addr, uint32_t value) {
		if (addr + 3 >= data.size()) throw out_of_range("Memory access out of range");
		data[addr] = value & 0xFF;
		data[addr + 1] = (value >> 8) & 0xFF;
		data[addr + 2] = (value >> 16) & 0xFF;
		data[addr + 3] = (value >> 24) & 0xFF;
	}

	uint8_t loadb(uint32_t addr) const { if (addr >= data.size()) throw out_of_range("Memory access out of range"); return data[addr]; }
	uint16_t loadh(uint32_t addr) const { if (addr + 1 >= data.size()) throw out_of_range("Memory access out of range"); return (data[addr]) | (data[addr + 1] << 8); }
	uint32_t loadw(uint32_t addr) const { if (addr + 3 >= data.size()) throw out_of_range("Memory access out of range"); return (data[addr]) | (data[addr + 1] << 8) | (data[addr + 2] << 16) | (data[addr + 3] << 24); }
	
	void printb() {
		for (size_t addr = 0; addr < data.size(); addr++) {
			cout << "0x" << setw(2) << setfill('0') << hex << (int)addr << ": 0x" << setw(2) << setfill('0') << (int)loadb(addr) << dec << endl;
		}
	}
	
	void printw() {
		for (size_t addr = 0; addr < data.size(); addr += 4) {
			if (loadw(addr) == 0) continue;
			cout << "0x" << setw(8) << setfill('0') << hex << (int)addr << ": 0x" << setw(8) << setfill('0') << (int)loadw(addr) << dec << endl;
		}
	}

private:
	vector<uint8_t> data;
};
