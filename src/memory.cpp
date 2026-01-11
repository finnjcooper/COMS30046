#include "memory.h"
#include <iomanip>

Memory::~Memory() = default;
Memory::Memory() { mem = unordered_map<uint8_t, uint8_t>(); }
Memory::Memory(vector<uint8_t> init) {
	mem = unordered_map<uint8_t, uint8_t>();
	cout << init.size() << endl;
	for (size_t i = 0; i < init.size(); i++) {
		mem[i] = init[i];
	}
}

void Memory::store(uint8_t addr, uint8_t value) { mem[addr] = value; }

uint8_t Memory::load8(uint8_t addr) {
	auto it = mem.find(addr);
	if (it != mem.end()) return it->second;
	return -1;
}

uint32_t Memory::load32(uint8_t addr) {
	uint32_t value = 0;
	for (int i = 0; i < 4; i++) {
		value |= (load8(addr + i) << (i * 8));
	}
	return value;
}

void Memory::print() {
	for (const auto& [addr, value] : mem) {
		cout << "0x" << setw(2) << setfill('0') << hex << (int)addr << ": 0x" << setw(2) << setfill('0') << (int)value << dec << endl;
	}
}