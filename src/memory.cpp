#include "memory.h"

Memory::~Memory() = default;
Memory::Memory(size_t size) { mem = vector<uint8_t>(size, 0); }
Memory::Memory(vector<uint8_t> init) { mem = init; }

void Memory::storeb(uint32_t addr, uint8_t value) { mem[addr] = value; }
void Memory::storeh(uint32_t addr, uint16_t value) {
	mem[addr]     = value & 0xFF;
	mem[addr + 1] = (value >> 8) & 0xFF;
}
void Memory::storew(uint32_t addr, uint32_t value) {
	mem[addr]     = value & 0xFF;
	mem[addr + 1] = (value >> 8) & 0xFF;
	mem[addr + 2] = (value >> 16) & 0xFF;
	mem[addr + 3] = (value >> 24) & 0xFF;
}

uint8_t Memory::loadb(uint32_t addr) { return mem[addr];}
uint16_t Memory::loadh(uint32_t addr) { return (mem[addr]) | (mem[addr + 1] << 8); }
uint32_t Memory::loadw(uint32_t addr) { return (mem[addr]) | (mem[addr + 1] << 8) | (mem[addr + 2] << 16) | (mem[addr + 3] << 24); }

void Memory::print8() {
	for (size_t addr = 0; addr < mem.size(); addr++) {
		cout << "0x" << setw(2) << setfill('0') << hex << (int)addr << ": 0x" << setw(2) << setfill('0') << loadb(addr) << dec << endl;
	}
}
void Memory::print32() {
	for (size_t addr = 0; addr < mem.size(); addr += 4) {
		cout << "0x" << setw(8) << setfill('0') << hex << (int)addr << ": 0x" << setw(8) << setfill('0') << loadw(addr) << dec << endl;
	}
}