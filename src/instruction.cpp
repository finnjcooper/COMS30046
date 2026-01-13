#include "instruction.h"

vector<uint8_t> Loader::fromBinary(const string& filename) {
	ifstream file(filename, ios::binary);
	auto bytes = vector<uint8_t>();
	char byte;
	while (file.read(&byte, 1)) {
		bytes.push_back(byte);
	};

	return bytes;
}