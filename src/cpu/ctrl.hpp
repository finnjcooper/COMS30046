#pragma once
#include "exec.hpp"

class ControlUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		int32_t imm = current.imm;
		Value alu_out = Value::scalar(0);
		uint32_t target = current.pc + imm;
		bool jumped = false;

		if (is_jump(op)) {
			alu_out = Value::scalar(current.pc + WORD_BYTES);
			jumped = true;
			if (op == JALR) target = (current.Vj.as_scalar() + imm) & ~1U;
		} else {
			Value ctrl_out = exec(op, current.Vj, current.Vk, current.Vl);
			jumped = ctrl_out.as_scalar() != 0;
		}

		busy_ = false;
		return ExecEntry {current.op, alu_out, target, jumped, current.tag};
	}

private:
	Value exec(Op op, Value v1, Value v2, Value v3) override {
		uint32_t val1 = v1.as_scalar(), val2 = v2.as_scalar();
		switch (op) {
			case BEQ: return Value::scalar(val1 == val2);
			case BNE: return Value::scalar(val1 != val2);
			case BLT: return Value::scalar(static_cast<int32_t>(val1) < static_cast<int32_t>(val2));
			case BGE: return Value::scalar(static_cast<int32_t>(val1) >= static_cast<int32_t>(val2));
			case BLTU: return Value::scalar(val1 < val2);
			case BGEU: return Value::scalar(val1 >= val2);
			default: return Value::scalar(0);
		}
	}
};
