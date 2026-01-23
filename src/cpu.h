#pragma once
#include <sstream>
#include "regfile.h"
#include "memory.h"
#include "alu.h"
#include "branch.h"
#include "loadstore.h"
#include "instruction.h"
#include "decoder.h"

class CPU {
public:
	~CPU() = default;
	CPU(Program prog) : mem(Memory(prog.instrs, MEM_SIZE)), pc(prog.entryPoint) { regs.write(2, MEM_SIZE - 4); }

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
	LoadStoreUnit lsu = LoadStoreUnit();
	BranchUnit bu = BranchUnit();
	Memory mem;

	bool pipelined = false;

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
			case Opcode::SLTU: case Opcode::XOR: case Opcode::SRL: case Opcode::OR: case Opcode::AND:
			case Opcode::MUL: case Opcode::MULH: case Opcode::MULHSU: case Opcode::MULHU:
			case Opcode::DIV: case Opcode::DIVU: case Opcode::REM: case Opcode::REMU: {
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
				uint32_t value = lsu.load(instr.op, mem, addr);
				regs.write(instr.rd, value);
				break;
			}
			case Opcode::SB: case Opcode::SH: case Opcode::SW: {
				uint32_t addr = regs.read(instr.rs1) + instr.imm;
				uint32_t value = regs.read(instr.rs2);
				lsu.store(instr.op, mem, addr, value);
				break;
			}
			case Opcode::BEQ: case Opcode::BNE: case Opcode::BLT: case Opcode::BGE: case Opcode::BLTU: case Opcode::BGEU: {
				int32_t val1 = regs.read(instr.rs1);
				int32_t val2 = regs.read(instr.rs2);
				bool branch = bu.evaluate(instr.op, val1, val2);
				if (branch) pc += instr.imm - 4;
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
