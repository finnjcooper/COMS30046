#include "registers.h"
#include "memory.h"
#include "alu.h"
#include "program.h"
#include <iostream>
#include <bitset>

using namespace std;

class CPU {
public:
	~CPU() = default;
	CPU() = default;
	CPU(vector<uint8_t> imem_init, vector<uint8_t> dmem_init) : imem(Memory(imem_init)), dmem(Memory(dmem_init)) {}

	RegisterFile regs = RegisterFile();
	ALU alu = ALU();
	Memory imem;
	Memory dmem;

	void step() {
		execute(decode(fetch()));
	}

	bool isRunning() { return !halted; }

private:
	uint8_t pc = 0;
	bool halted = false;

	int fetch() {
		return imem.load32(pc += 4);
	}

	Instruction decode(uint32_t instruction) {
		bitset<32> bits(instruction);
		cout << bits << endl;
		uint32_t op = instruction & 0x0000007F;
		switch (op) {
			// case 0x00: return {Opcode::ADD, (instruction >> 7) & 0x0F, (instruction >> 10) & 0x0F, 0};
			default: return {Opcode::NOP, 0, 0, 0};
		}
	}

	void execute(Instruction instr) {
		switch (instr.op)
		{
			case Opcode::HALT: halted = true;
			case Opcode::NOP: {
				break;
			}
			case Opcode::ADD: {
				int result = alu.execute(
					Opcode::ADD,
					regs.read(instr.rs1),
					regs.read(instr.rs2)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::ADDI: {
				int result = alu.execute(
					Opcode::ADD,
					regs.read(instr.rs1),
					instr.imm
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::SUB: {
				int result = alu.execute(
					Opcode::SUB,
					regs.read(instr.rs1),
					regs.read(instr.rs2)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::SUBI: {
				int result = alu.execute(
					Opcode::SUB,
					regs.read(instr.rs1),
					instr.imm
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::CMP: {
				int result = alu.execute(
					Opcode::CMP,
					regs.read(instr.rs1),
					regs.read(instr.rs2)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::LD: {
				int value = dmem.load32(instr.imm);
				regs.write(instr.rd, value);
				break;
			}
			case Opcode::LDI: {
				regs.write(instr.rd, instr.imm);
				break;
			}
			case Opcode::ST: {
				int value = regs.read(instr.rs1);
				dmem.store(regs.read(instr.rd), value);
				break;
			}
			case Opcode::STI: {
				int value = regs.read(instr.rs1);
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
				if (regs.read(instr.rs1) == 0) {
					pc = instr.imm;
				}
				break;
			}
			case Opcode::BLT: {
				if (regs.read(instr.rs1) < 0) {
					pc = instr.imm;
				}
				break;
			}
			case Opcode::BGT: {
				if (regs.read(instr.rs1) > 0) {
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
	// vector<Instruction> prog = Loader::sampleProgram();
	vector<uint8_t> raw = Loader::fromBinary("build/bench.bin");

	CPU cpu(raw, {});
	cpu.imem.print();

	cout << endl << "Starting CPU... Press enter to step." << endl;
	
	while (cpu.isRunning()) {
		cin.get();
		cpu.step();
		for (int i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
			cout << "x" << i << ": " << cpu.regs.read(i) << " ";
		}
	}

	return 0;
}