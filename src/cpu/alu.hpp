#pragma once
#include "exec.hpp"

class ALU : public ExecUnit {
public:
	void step() override {
		if (!busy_) return;

		cycles_remaining--;
		if (cycles_remaining == 0) {
			Op op = current.op;
			uint32_t r1 = current.Vj, r2 = current.Vk;

			if (isUI(op) || isALUI(op)) r2 = current.imm;
			if (op == LUI)              r1 = 0U;
			if (op == AUIPC)            r1 = current.pc;

			uint32_t alu_out = exec(op, r1, r2);
			result = {current.op, alu_out, 0, 0, false, false, current.tag};
			busy_ = false; done_ = true;
		}
	}

private:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2) override {
		
		if (isLoad(op) || isStore(op)) return operand1 + operand2;

		switch (op) {
			case ADD: case ADDI: case LUI: case AUIPC:
				return operand1 + operand2;
			case SUB:
				return operand1 - operand2;
			case SLL: case SLLI:
				return operand1 << (operand2 & 0x1F);
			case SLT: case SLTI:
				return (static_cast<int32_t>(operand1) < static_cast<int32_t>(operand2)) ? 1 : 0;
			case SLTU: case SLTIU:
				return (operand1 < operand2) ? 1 : 0;
			case XOR: case XORI:
				return operand1 ^ operand2;
			case SRL: case SRLI:
				return operand1 >> (operand2 & 0x1F);
			case SRA: case SRAI:
				return static_cast<int32_t>(operand1) >> (operand2 & 0x1F);
			case OR: case ORI:
				return operand1 | operand2;
			case AND: case ANDI:
				return operand1 & operand2;
			default:
				return 0;
		}
	}
};
