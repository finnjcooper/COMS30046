#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <bitset>
#include <iostream>

using namespace std;

class Memory {
public:
	Memory();
	Memory(vector<uint8_t> init);
	~Memory();

	void store(uint8_t addr, uint8_t value);
	uint8_t load8(uint8_t addr);
	uint32_t load32(uint8_t addr);
	void print();

private:
	unordered_map<uint8_t, uint8_t> mem;
};