#pragma once
#include <sstream>
#include "regfile.h"
#include "memory.h"
#include "alu.h"
#include "instruction.h"
#include "decoder.h"

class CPU {
public:
	~CPU() = default;
	CPU(Program prog) : mem(Memory(prog.instrs, MEM_SIZE)), pc(prog.entryPoint) { regs.write(2, MEM_SIZE); }

	static constexpr size_t MEM_SIZE = 64 * 1024; // 64 KB

	void step() { execute(decode(fetch())); }
	bool isRunning() { return !halted; }
	uint32_t getPC() const { return pc; }
	RegisterFile getRegisters() const { return regs; }
	Memory getMemory() const { return mem; }
	
	string readout() {
		string s = out.str();
		out.str("");
		out.clear();
		return s;
	}

private:
	RegisterFile regs = RegisterFile();
	ALU alu = ALU();
	Memory mem;

	ostringstream out;

	uint32_t pc = 0;
	bool halted = false;

	uint32_t fetch() {
		uint32_t instr = mem.loadw(pc);
		pc += 4;
		return instr;
	}

	Instruction decode(uint32_t instruction) {
		return Decoder::decode(instruction);
	}
	
	void execute(Instruction instr) {
		switch (instr.op) {
			case Opcode::ECALL:
			case Opcode::EBREAK:
				halted = true;
				break;
			case Opcode::SUB: case Opcode::SRA: case Opcode::ADD: case Opcode::SLL: case Opcode::SLT:
			case Opcode::SLTU: case Opcode::XOR: case Opcode::SRL: case Opcode::OR: case Opcode::AND: {
				uint32_t result = alu.execute(
					instr.op,
					regs.read(instr.rs1),
					regs.read(instr.rs2)
				);
				regs.write(instr.rd, result);
				break;
			}
			case Opcode::ADDI: case Opcode::SLLI: case Opcode::SLTI: case Opcode::SLTIU: case Opcode::XORI:
			case Opcode::SRAI: case Opcode::SRLI: case Opcode::ORI: case Opcode::ANDI: {
				uint32_t result = alu.execute(
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
				if (instr.op == Opcode::LB) value = Decoder::sign_extend(mem.loadb(addr), 8);
				if (instr.op == Opcode::LH) value = Decoder::sign_extend(mem.loadh(addr), 16);
				if (instr.op == Opcode::LW) value = mem.loadw(addr);
				if (instr.op == Opcode::LBU) value = mem.loadb(addr);
				if (instr.op == Opcode::LHU) value = mem.loadh(addr);
				regs.write(instr.rd, value);
				break;
			}
			case Opcode::SB: case Opcode::SH: case Opcode::SW: {
				uint32_t addr = regs.read(instr.rs1) + instr.imm;
				uint32_t value = regs.read(instr.rs2);
				if (instr.op == Opcode::SB) mem.storeb(addr, value & 0xFF);
				if (instr.op == Opcode::SH) mem.storeh(addr, value & 0xFFFF);
				if (instr.op == Opcode::SW) mem.storew(addr, value);
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
				if (take_branch) pc += instr.imm - 4;
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
			case Opcode::INVALID:
				out << "Invalid instruction encountered!" << endl;
			default:
				return;
		}
	}
};
