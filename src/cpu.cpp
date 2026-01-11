#include "registers.h"
#include "memory.h"
#include "alu.h"
#include "program.h"
#include <iostream>

using namespace std;

class CPU {
public:
	CPU(Program &imem, Memory &dmem) : imem(imem), dmem(dmem) {};
	~CPU() = default;

	RegisterFile regs = RegisterFile();
	ALU alu = ALU();
	Program &imem;
	Memory &dmem;

	void step() {
		execute(fetch());
	}

	bool isHalted() { return halted; }

private:
	int pc = 0;
	bool halted = false;

	Instruction fetch() {
		return imem.instructions[pc++];
	}

	// Instruction decode(int instruction) {
		
	// }

	void execute(Instruction instr) {
		switch (instr.op)
		{
			case Opcode::HALT: {
				halted = 1;
				break;
			}
			case Opcode::ADD: {
				int result = alu.execute(
					Opcode::ADD,
					regs.read(instr.rd),
					regs.read(instr.rs)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::ADDI: {
				int result = alu.execute(
					Opcode::ADD,
					regs.read(instr.rd),
					instr.imm
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::SUB: {
				int result = alu.execute(
					Opcode::SUB,
					regs.read(instr.rd),
					regs.read(instr.rs)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::SUBI: {
				int result = alu.execute(
					Opcode::SUB,
					regs.read(instr.rd),
					instr.imm
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::CMP: {
				int result = alu.execute(
					Opcode::CMP,
					regs.read(instr.rd),
					regs.read(instr.rs)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::LD: {
				int value = dmem.load(instr.imm);
				regs.write(instr.rd, value);
				break;
			}
			case Opcode::LDI: {
				regs.write(instr.rd, instr.imm);
				break;
			}
			case Opcode::ST: {
				int value = regs.read(instr.rs);
				dmem.store(regs.read(instr.rd), value);
				break;
			}
			case Opcode::STI: {
				int value = regs.read(instr.rs);
				dmem.store(instr.imm, value);
				break;
			}
			case Opcode::JMP: {
				pc += instr.imm;
				break;
			}
			case Opcode::BR: {
				pc = instr.imm;
				break;
			}
			case Opcode::BEQ: {
				if (regs.read(instr.rs) == 0) {
					pc = instr.imm;
				}
				break;
			}
			case Opcode::BLT: {
				if (regs.read(instr.rs) < 0) {
					pc = instr.imm;
				}
				break;
			}
			case Opcode::BGT: {
				if (regs.read(instr.rs) > 0) {
					pc = instr.imm;
				}
				break;
			}
			default:
				cout << "Unknown opcode encountered!" << endl;
				break;
		}
	}
};

int main() {
	Program prog = Loader::sampleProgram();
	CPU cpu(prog, *(new Memory()));
	
	while (!cpu.isHalted()) {
		cpu.step();
		for (int i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
			cout << "R" << i << ": " << cpu.regs.read(i) << " ";
		}
		cout << endl;
	}

	return 0;
}