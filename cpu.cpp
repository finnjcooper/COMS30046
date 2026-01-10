#include "memory.h"
#include "program.h"
#include <iostream>

using namespace std;

class CPU {
public:
	CPU(Memory &imem, Memory &dmem) : imem(imem), dmem(dmem) {};
	~CPU() = default;

	Memory &imem;
	Memory &dmem;
};

int main() {
	CPU cpu(*(new Memory()), *(new Memory()));
	for (const auto& instr : Loader::sampleProgram().instructions) {
		cpu.imem.store(&instr - &Loader::sampleProgram().instructions[0], static_cast<int>(instr.op));
	}
	
	for (int i = 0; i < Loader::sampleProgram().instructions.size(); i++) {
		int opcode = cpu.imem.load(i);
		cout << "Instruction " << i << ": Opcode " << opcode << endl;
	}

	return 0;
}