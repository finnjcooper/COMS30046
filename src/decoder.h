#pragma once
#include "instruction.h"
#include <bitset>
#include <iomanip>

class Decoder {
public:
	static Instruction decode(uint32_t instruction);
	static inline int32_t sign_extend(uint32_t value, int bits) {
		int32_t shift = 32 - bits;
		return (int32_t)(value << shift) >> shift;
	}
};