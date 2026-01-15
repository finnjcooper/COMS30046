#pragma once
#include <cstdint>
#include <stdexcept>

struct RegValue {
	uint32_t prev = 0;
	uint32_t curr = 0;
};

class RegisterFile {
public:
	static const uint8_t NUM_REGISTERS = 32;
	RegValue registers[NUM_REGISTERS];

	uint32_t read(uint8_t index) const {
		if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
		return registers[index].curr;
	}

	uint32_t readPrev(uint8_t index) const {
		if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
		return registers[index].prev;
	}

	void write(uint8_t index, uint32_t value) {
		if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
		else if (index == 0) return; // discard writes to x0
		registers[index].curr = value;
	}

	void commit() {
		for (int i = 0; i < NUM_REGISTERS; i++) registers[i].prev = registers[i].curr;
	}
};
