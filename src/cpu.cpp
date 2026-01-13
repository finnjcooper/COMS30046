#include "cpu.h"

void CPU::step() {
	execute(decode(fetch()));
}

int CPU::fetch() {
	return imem.load32(pc += 4);
}

Instruction CPU::decode(uint32_t instruction) {
	return Decoder::decode(instruction);
}

void CPU::execute(Instruction instr) {
	if (instr.rd == 2) {
		// stack pointer
	};

	switch (instr.op) {
		case Opcode::ECALL:
		case Opcode::EBREAK: halted = true;
		case Opcode::INVALID: {
			break;
		}
		case Opcode::ADD: {
			cout << "Executing ADD of registers " << (int)instr.rs1 << " and " << (int)instr.rs2 << " to register " << (int)instr.rd << endl;
			int result = alu.execute(
				Opcode::ADD,
				regs.read(instr.rs1),
				regs.read(instr.rs2)
			);
			regs.write(instr.rd, result);
			break;
		}
		case Opcode::ADDI: {
			cout << "Executing ADD of 0x" << instr.imm << " to register " << (int)instr.rd << endl;
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
		default: {
			cout << "Unknown opcode encountered!" << endl;
			break;
		}
	}
}
