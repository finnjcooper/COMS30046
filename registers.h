#pragma once

class RegisterFile {
public:
	static const int NUM_REGISTERS = 16;
	int registers[NUM_REGISTERS] = {0};

	int read(int index);
	void write(int index, int value);
};