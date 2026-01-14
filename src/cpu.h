#pragma once
#include "registers.h"
#include "memory.h"
#include "alu.h"
#include "instruction.h"
#include "decoder.h"
#include <bitset>

class CPU {
public:
	~CPU() = default;
	CPU(vector<uint8_t> imem_init) : imem(Memory(imem_init)), dmem(Memory(64 * 1024)) {}
	CPU(vector<uint8_t> imem_init, vector<uint8_t> dmem_init) : imem(Memory(imem_init)), dmem(Memory(dmem_init)) {}

	RegisterFile regs = RegisterFile();
	ALU alu = ALU();
	Stack stack = Stack();
	Memory imem;
	Memory dmem;

	void step();
	bool isRunning() { return !halted; }

private:
	uint32_t pc = 0;
	bool halted = false;

	int fetch();
	Instruction decode(uint32_t instruction);
	void execute(Instruction instr);
};