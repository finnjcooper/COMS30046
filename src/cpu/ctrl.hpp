#pragma once
#include "exec.hpp"

class ControlUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		uint32_t r1 = current.Vj, r2 = current.Vk, r3 = current.Vl;
		int32_t imm = current.imm;
		uint32_t alu_out = 0;
		uint32_t target = current.pc + imm;
		bool jumped = false;

		if (is_jump(op)) {
			alu_out = current.pc + WORD_BYTES;
			jumped = true;
			if (op == JALR) target = (r1 + imm) & ~1U;
		} else {
			jumped = exec(op, r1, r2, r3);
		}

		uint32_t next_pc = jumped ? target : current.pc + WORD_BYTES;

		busy_ = false;
		return ExecEntry {current.op, alu_out, 0, target, jumped, current.tag};
	}

private:
	uint32_t exec(Op op, uint32_t val1, uint32_t val2, uint32_t val3) override {
		switch (op) {
			case BEQ: return val1 == val2;
			case BNE: return val1 != val2;
			case BLT: return static_cast<int32_t>(val1) < static_cast<int32_t>(val2);
			case BGE: return static_cast<int32_t>(val1) >= static_cast<int32_t>(val2);
			case BLTU: return val1 < val2;
			case BGEU: return val1 >= val2;
			default: return 0;
		}
	}
};
