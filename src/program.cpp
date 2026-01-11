#include "program.h"

vector<Instruction> Loader::sampleProgram() {
	return vector<Instruction>({
		{LDI, 1, 0, 0, 0},    // Load immediate value 0 into R1
		{LDI, 2, 0, 0, 10},   // Load immediate value 10 into R2
		{ADD, 1, 1, 2, 0},    // R1 = R1 + R2
		{STI, 0, 1, 0, 100},  // Store value of R1 into memory address 100
		{LD, 3, 0, 0, 100},   // Load value from memory address 100 into R3
		{SUBI, 3, 3, 5, 0},   // R3 = R3 - 5
		{BEQ, 0, 3, 0, 8},    // If R3 == 0, branch to instruction at index 8
		{BR, 0, 0, 0, 5},     // Jump back to instruction at index 5
		{HALT, 0, 0, 0, 0}    // Halt execution
	});
};

vector<uint8_t> Loader::fromBinary(const string& filename) {
	ifstream file(filename, ios::binary);
	auto bytes = vector<uint8_t>();
	char byte;
	while (file.read(&byte, 1)) {
		bytes.push_back(byte);
	};

	return bytes;
}