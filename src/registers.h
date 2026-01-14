#pragma once
#include <cstdint>
#include <stdexcept>

class RegisterFile {
public:
	static const int NUM_REGISTERS = 32;
	int registers[NUM_REGISTERS] = {0};

	int read(uint8_t index) {
		if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
		return registers[index];
	}

	void write(uint8_t index, uint32_t value) {
		if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
		else if (index == 0) return; // discard writes to x0
		registers[index] = value;
	}
};
