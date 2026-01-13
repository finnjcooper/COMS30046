#pragma once
#include "instruction.h"
#include <bitset>
#include <iomanip>

class Decoder {
public:
	static Instruction decode(uint32_t instruction);
};