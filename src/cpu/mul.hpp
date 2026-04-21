#pragma once
#include <limits>
#include "exec.hpp"

class MulDivUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		uint32_t mul_out = exec(current.op, current.Vj, current.Vk, current.Vl);
		busy_ = false;
		return ExecEntry {current.op, mul_out, 0, 0, false, current.tag};
	}

private:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2, uint32_t operand3) override {
		switch (op) {
			case MUL: case MULH: {
				int64_t result = static_cast<int64_t>(static_cast<int32_t>(operand1)) * static_cast<int64_t>(static_cast<int32_t>(operand2));
				return op == MUL ? static_cast<uint32_t>(result) : static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32);
			}
			case MULHSU: {
				int64_t result = static_cast<int64_t>(static_cast<int32_t>(operand1)) * static_cast<uint64_t>(operand2);
				return static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32);
			}
			case MULHU: {
				uint64_t result = static_cast<uint64_t>(operand1) * static_cast<uint64_t>(operand2);
				return static_cast<uint32_t>(result >> 32);
			}
			case DIV: {
				int32_t lhs = static_cast<int32_t>(operand1), rhs = static_cast<int32_t>(operand2);
				if (operand2 == 0) return 0xFFFFFFFF;
				if (lhs == numeric_limits<int32_t>::min() && rhs == -1) return static_cast<uint32_t>(lhs);
				return static_cast<uint32_t>(lhs / rhs);
			}
			case REM: {
				int32_t lhs = static_cast<int32_t>(operand1), rhs = static_cast<int32_t>(operand2);
				if (operand2 == 0) return operand1;
				if (lhs == numeric_limits<int32_t>::min() && rhs == -1) return 0;
				return static_cast<uint32_t>(lhs % rhs);
			}
			case DIVU:
				if (operand2 == 0) return 0xFFFFFFFF;
				return operand1 / operand2;
			case REMU:
				if (operand2 == 0) return operand1;
				return operand1 % operand2;
			default:
				return 0;
		}
	}
};
