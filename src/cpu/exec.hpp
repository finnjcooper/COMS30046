#pragma once
#include "instruction.hpp"

class ExecUnit {
public:
	virtual ~ExecUnit() = default;
	virtual uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) = 0;
};
