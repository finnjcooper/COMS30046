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
	CPU(Program prog) : imem(Memory(prog.memory)), dmem(Memory(64 * 1024)), pc(prog.entryPoint) { regs.write(2, 64 * 1024); }
	CPU(Program prog, vector<uint8_t> data) : imem(Memory(prog.memory)), dmem(Memory(data)), pc(prog.entryPoint) { regs.write(2, data.size()); }

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