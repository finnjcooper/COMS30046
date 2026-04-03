#pragma once
#include "exec.hpp"

class ControlUnit : public ExecUnit {
public:
	ControlUnit(uint8_t word_bytes) : WORD_BYTES(word_bytes) {}

	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		uint32_t r1 = current.Vj, r2 = current.Vk;
		int32_t imm = current.imm;
		uint32_t alu_out = 0;
		uint32_t target = current.pc + imm;
		bool jumped = false, should_halt = false;

		if (is_jump(op)) {
			alu_out = current.pc + WORD_BYTES;
			jumped = true;
			if (op == JALR) target = (r1 + imm) & ~1U;
		} else if (op == ECALL) {
			should_halt = true;
		} else {
			jumped = exec(op, r1, r2);
		}

		busy_ = false;
		return ExecEntry {current.op, alu_out, 0, target, jumped, should_halt, current.tag};
	}

private:
	uint8_t WORD_BYTES;

	uint32_t exec(Op op, uint32_t val1, uint32_t val2) override {
		switch (op) {
			case BEQ:
				return val1 == val2;
			case BNE:
				return val1 != val2;
			case BLT:
				return static_cast<int32_t>(val1) < static_cast<int32_t>(val2);
			case BGE:
				return static_cast<int32_t>(val1) >= static_cast<int32_t>(val2);
			case BLTU:
				return val1 < val2;
			case BGEU:
				return val1 >= val2;
			default:
				return 0;
		}
	}
};
