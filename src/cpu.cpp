#include "cpu.h"

void CPU::step() {
	execute(decode(fetch()));
}

int CPU::fetch() {
	uint32_t instr = imem.loadw(pc);
    pc += 4;
    return instr;
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
		case Opcode::EBREAK:
			halted = true;
			break;
		case Opcode::SUB: case Opcode::SRA: case Opcode::ADD: case Opcode::SLL: case Opcode::SLT:
		case Opcode::SLTU: case Opcode::XOR: case Opcode::SRL: case Opcode::OR: case Opcode::AND: {
			int result = alu.execute(
				instr.op,
				regs.read(instr.rs1),
				regs.read(instr.rs2)
			);
			regs.write(instr.rd, result);
			break;
		}
		case Opcode::ADDI: case Opcode::SLLI: case Opcode::SLTI: case Opcode::SLTIU: case Opcode::XORI:
		case Opcode::SRAI: case Opcode::SRLI: case Opcode::ORI: case Opcode::ANDI: {
			int result = alu.execute(
				instr.op,
				regs.read(instr.rs1),
				instr.imm
			);
			regs.write(instr.rd, result);
			break;
		}
		case Opcode::LB: case Opcode::LH: case Opcode::LW: case Opcode::LBU: case Opcode::LHU: {
			uint32_t addr = regs.read(instr.rs1) + instr.imm;
			uint32_t value = 0;
			if (instr.op == Opcode::LB) value = Decoder::sign_extend(dmem.loadb(addr), 8);
			if (instr.op == Opcode::LH) value = Decoder::sign_extend(dmem.loadh(addr), 16);
			if (instr.op == Opcode::LW) value = dmem.loadw(addr);
			if (instr.op == Opcode::LBU) value = dmem.loadb(addr);
			if (instr.op == Opcode::LHU) value = dmem.loadh(addr);
			regs.write(instr.rd, value);
			break;
		}
		case Opcode::SB: case Opcode::SH: case Opcode::SW: {
			uint32_t addr = regs.read(instr.rs1) + instr.imm;
			uint32_t value = regs.read(instr.rs2);
			if (instr.op == Opcode::SB) dmem.storeb(addr, value & 0xFF);
			if (instr.op == Opcode::SH) dmem.storeh(addr, value & 0xFFFF);
			if (instr.op == Opcode::SW) dmem.storew(addr, value);
			break;
		}
		case Opcode::BEQ: case Opcode::BNE: case Opcode::BLT: case Opcode::BGE: case Opcode::BLTU: case Opcode::BGEU: {
			int32_t val1 = regs.read(instr.rs1);
			int32_t val2 = regs.read(instr.rs2);
			bool take_branch = false;
			if (instr.op == Opcode::BEQ) take_branch = (val1 == val2);
			if (instr.op == Opcode::BNE) take_branch = (val1 != val2);
			if (instr.op == Opcode::BLT) take_branch = (val1 < val2);
			if (instr.op == Opcode::BGE) take_branch = (val1 >= val2);
			if (instr.op == Opcode::BLTU) take_branch = ((uint32_t)val1 < (uint32_t)val2);
			if (instr.op == Opcode::BGEU) take_branch = ((uint32_t)val1 >= (uint32_t)val2);
			if (take_branch) {
				pc += instr.imm - 4;
			}
			break;
		}
		case Opcode::JAL: {
			regs.write(instr.rd, pc);
			pc += instr.imm - 4;
			break;
		}
		case Opcode::JALR: {
			uint32_t temp = pc;
			pc = (regs.read(instr.rs1) + instr.imm) & ~1;
			regs.write(instr.rd, temp);
			break;
		}
		case Opcode::LUI: case Opcode::AUIPC: {
			regs.write(instr.rd, instr.imm + ((pc - 4) * (instr.op == Opcode::AUIPC)));
			break;
		}
		default: {
			return;
		}
		case Opcode::INVALID:
			return;
	}

	cout << "Executed instruction at PC=0x" << hex << pc - 4 << ": opcode=" << hex << (int)instr.op << dec << ", rd=" << (int)instr.rd << ", rs1=" << (int)instr.rs1 << ", rs2=" << (int)instr.rs2 << ", imm=" << dec << instr.imm << endl;
}
