#include <stdexcept>
#include "registers.h"

int RegisterFile::read(int index) {
	if (index < 0 || index >= NUM_REGISTERS) {
		throw std::out_of_range("Register index out of range");
	}
	return registers[index];
}

void RegisterFile::write(int index, int value) {
	if (index < 0 || index >= NUM_REGISTERS) {
		throw std::out_of_range("Register index out of range");
	}
	registers[index] = value;
}