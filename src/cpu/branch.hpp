#pragma once
#include "exec.hpp"

class BranchUnit : public ExecUnit {
public:
	BranchUnit(uint32_t end, uint8_t word_bytes) : end(end), WORD_BYTES(word_bytes) {}

	void step() override {
		if (!busy_) return;

		cycles_remaining--;
		if (cycles_remaining == 0) {
			Op op = current.op;
			uint32_t r1 = current.Vj, r2 = current.Vk, imm = current.imm;
			uint32_t alu_out = 0;
			uint32_t target = current.pc + imm;
			
			bool bru_out = exec(op, r1, r2);
			
			if (isJAL(op)) {
				alu_out = current.pc + WORD_BYTES;
				if (op == JALR) target = (r1 + imm) & ~1U;
			}

			result = {current.op, alu_out, 0, target, bru_out, bru_out && target >= end, current.tag};
			busy_ = false; done_ = true;
		}
	}

private:
	uint32_t end;
	uint8_t WORD_BYTES;

	uint32_t exec(Op op, uint32_t val1, uint32_t val2) override {
		if (isJAL(op)) return true;
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
