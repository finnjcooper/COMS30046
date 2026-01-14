#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <bitset>
#include <iostream>
#include <iomanip>

using namespace std;

class Memory {
public:
	Memory(size_t size);
	Memory(vector<uint8_t> init);
	~Memory();

	void storeb(uint32_t addr, uint8_t value);
	void storeh(uint32_t addr, uint16_t value);
	void storew(uint32_t addr, uint32_t value);
	uint8_t loadb(uint32_t addr);
	uint16_t loadh(uint32_t addr);
	uint32_t loadw(uint32_t addr);
	void print8();
	void print32();

private:
	vector<uint8_t> mem;
};

class Stack {
public:
	~Stack() = default;
	Stack() = default;

	void push(uint8_t value) { data.push_back(value); }
	uint8_t pop() {
		if (data.empty()) {
			cerr << "Stack underflow!" << endl;
			return 0;
		}
		uint8_t value = data.back();
		data.pop_back();
		return value;
	}

private:
	vector<uint8_t> data;
};