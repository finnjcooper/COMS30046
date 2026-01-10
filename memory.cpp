#include "memory.h"

Memory::Memory() { mem = unordered_map<int, int>(); }
Memory::~Memory() = default;

void Memory::store(int addr, int value) { mem[addr] = value; }
int Memory::load(int addr) {
	auto it = mem.find(addr);
	if (it != mem.end()) return it->second;
	return -1;
}