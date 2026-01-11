#include "registers.h"

int RegisterFile::read(uint8_t index) {
	if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
	return registers[index];
}

void RegisterFile::write(uint8_t index, uint32_t value) {
	if (index < 0 || index >= NUM_REGISTERS) throw std::out_of_range("Register index out of range");
	else if (index == 0) return; // discard writes to R0
	registers[index] = value;
}