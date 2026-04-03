#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <iostream>

using namespace std;

struct Program {
	vector<uint8_t> instrs;
	uint32_t entry_point;
};

class Loader {
public:
	static Program ELF(const string &filename);
	static map<uint32_t, string> ASM(const string &filename);
};
