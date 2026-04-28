#pragma once
#include "exec.hpp"

class ArithmeticLogicUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		Value v1 = current.Vj, v2 = current.Vk, v3 = current.Vl;

		if (is_ui(op) || is_alui(op)) v2 = Value::scalar(current.imm);
		if (op == LUI)                v1 = Value::scalar(0);
		if (op == AUIPC)              v1 = Value::scalar(current.pc);

		Value alu_out = exec(op, v1, v2, v3);
		busy_ = false;
		return ExecEntry {current.op, alu_out, 0, false, current.tag};
	}

private:
	Value exec(Op op, Value v1, Value v2, Value v3) override {
		uint32_t operand1 = v1.as_scalar(), operand2 = v2.as_scalar(), operand3 = v3.as_scalar();
		switch (op) {
			case ADD: case ADDI: case LUI: case AUIPC:
				return Value::scalar(operand1 + operand2);
			case SUB:
				return Value::scalar(operand1 - operand2);
			case SLL: case SLLI:
				return Value::scalar(operand1 << (operand2 & 0x1F));
			case SLT: case SLTI:
				return Value::scalar((static_cast<int32_t>(operand1) < static_cast<int32_t>(operand2)) ? 1 : 0);
			case SLTU: case SLTIU:
				return Value::scalar((operand1 < operand2) ? 1 : 0);
			case XOR: case XORI:
				return Value::scalar(operand1 ^ operand2);
			case SRL: case SRLI:
				return Value::scalar(operand1 >> (operand2 & 0x1F));
			case SRA: case SRAI:
				return Value::scalar(static_cast<int32_t>(operand1) >> (operand2 & 0x1F));
			case OR: case ORI:
				return Value::scalar(operand1 | operand2);
			case AND: case ANDI:
				return Value::scalar(operand1 & operand2);
			default:
				return Value::scalar(0);
		}
	}
};
