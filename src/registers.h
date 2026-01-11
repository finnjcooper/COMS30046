#pragma once
#include <cstdint>
#include <stdexcept>

class RegisterFile {
public:
	static const int NUM_REGISTERS = 32;
	int registers[NUM_REGISTERS] = {0};

	int read(uint8_t index);
	void write(uint8_t index, uint32_t value);
};