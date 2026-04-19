#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include "exec.hpp"

class FloatingPointUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Op op = current.op;
		uint32_t r1 = current.Vj, r2 = current.Vk, r3 = current.Vl;

		uint32_t float_out = exec(op, r1, r2, r3);
		busy_ = false;
		return ExecEntry {current.op, float_out, 0, 0, false, false, current.tag};
	}

private:
	uint32_t exec(Op op, uint32_t operand1, uint32_t operand2, uint32_t operand3) override {
		float f1 = bits_to_float(operand1);
		float f2 = bits_to_float(operand2);
		float f3 = bits_to_float(operand3);

		switch (op) {
			case FADD_S: return float_to_bits(f1 + f2);
			case FSUB_S: return float_to_bits(f1 - f2);
			case FMUL_S: return float_to_bits(f1 * f2);
			case FDIV_S: return float_to_bits(f1 / f2);
			case FSQRT_S: return float_to_bits(sqrt(f1));
			case FMADD_S: return float_to_bits(fma(f1, f2, f3));
			case FMSUB_S: return float_to_bits(fma(f1, f2, -f3));
			case FNMSUB_S: return float_to_bits(fma(-f1, f2, f3));
			case FNMADD_S: return float_to_bits(fma(-f1, f2, -f3));
			case FSGNJ_S: return (operand1 & ~SIGN_MASK) | (operand2 & SIGN_MASK);
			case FSGNJN_S: return (operand1 & ~SIGN_MASK) | (~operand2 & SIGN_MASK);
			case FSGNJX_S: return (operand1 & ~SIGN_MASK) | ((operand1 ^ operand2) & SIGN_MASK);
			case FMIN_S: return fmin(operand1, operand2);
			case FMAX_S: return fmax(operand1, operand2);
			case FEQ_S: return compare_eq(operand1, operand2);
			case FLT_S: return compare_lt(operand1, operand2);
			case FLE_S: return compare_le(operand1, operand2);
			case FMV_X_W: return operand1;
			case FMV_W_X: return operand1;
			case FCVT_W_S: return fcvt_w_s(f1, current.rm);
			case FCVT_WU_S: return fcvt_wu_s(f1, current.rm);
			case FCVT_S_W: return float_to_bits(round_to_float(bits_to_i32(operand1), current.rm));
			case FCVT_S_WU: return float_to_bits(round_to_float(operand1, current.rm));
			case FCLASS_S: return fclass(operand1);
			default: return 0;
		}
	}

	static constexpr uint32_t SIGN_MASK = 0x80000000U;
	static constexpr uint32_t EXP_MASK = 0x7f800000U;
	static constexpr uint32_t FRAC_MASK = 0x007fffffU;
	static constexpr uint32_t QUIET_NAN_BIT = 0x00400000U;
	static constexpr uint32_t CANONICAL_NAN = 0x7fc00000U;
	static constexpr uint8_t RM_RNE = 0;
	static constexpr uint8_t RM_RTZ = 1;
	static constexpr uint8_t RM_RDN = 2;
	static constexpr uint8_t RM_RUP = 3;
	static constexpr uint8_t RM_RMM = 4;

	static float bits_to_float(uint32_t bits) {
		static_assert(sizeof(float) == sizeof(uint32_t));
		float value;
		memcpy(&value, &bits, sizeof(value));
		return value;
	}

	static uint32_t float_to_bits(float value) {
		static_assert(numeric_limits<float>::is_iec559);
		uint32_t bits;
		memcpy(&bits, &value, sizeof(bits));
		return is_nan(bits) ? CANONICAL_NAN : bits;
	}

	static int32_t bits_to_i32(uint32_t bits) {
		int32_t value;
		memcpy(&value, &bits, sizeof(value));
		return value;
	}

	static bool is_nan(uint32_t bits) {
		return (bits & EXP_MASK) == EXP_MASK && (bits & FRAC_MASK) != 0;
	}

	static bool is_zero(uint32_t bits) {
		return (bits & ~SIGN_MASK) == 0;
	}

	static uint32_t fmin(uint32_t operand1, uint32_t operand2) {
		bool op1_nan = is_nan(operand1);
		bool op2_nan = is_nan(operand2);
		if (op1_nan && op2_nan) return CANONICAL_NAN;
		if (op1_nan) return operand2;
		if (op2_nan) return operand1;
		if (is_zero(operand1) && is_zero(operand2)) return operand1 | operand2;

		return bits_to_float(operand1) < bits_to_float(operand2) ? operand1 : operand2;
	}

	static uint32_t fmax(uint32_t operand1, uint32_t operand2) {
		bool op1_nan = is_nan(operand1);
		bool op2_nan = is_nan(operand2);
		if (op1_nan && op2_nan) return CANONICAL_NAN;
		if (op1_nan) return operand2;
		if (op2_nan) return operand1;
		if (is_zero(operand1) && is_zero(operand2)) return operand1 & operand2;

		return bits_to_float(operand1) > bits_to_float(operand2) ? operand1 : operand2;
	}

	static uint32_t compare_eq(uint32_t operand1, uint32_t operand2) {
		if (is_nan(operand1) || is_nan(operand2)) return 0;
		return bits_to_float(operand1) == bits_to_float(operand2);
	}

	static uint32_t compare_lt(uint32_t operand1, uint32_t operand2) {
		if (is_nan(operand1) || is_nan(operand2)) return 0;
		return bits_to_float(operand1) < bits_to_float(operand2);
	}

	static uint32_t compare_le(uint32_t operand1, uint32_t operand2) {
		if (is_nan(operand1) || is_nan(operand2)) return 0;
		return bits_to_float(operand1) <= bits_to_float(operand2);
	}

	static uint8_t static_rm(uint8_t rm) {
		// Dynamic rounding needs frm CSR support; use RNE until CSRs exist.
		return rm <= RM_RMM ? rm : RM_RNE;
	}

	static double round_to_integral(double value, uint8_t rm) {
		switch (static_rm(rm)) {
			case RM_RTZ:
				return value < 0.0 ? ceil(value) : floor(value);
			case RM_RDN:
				return floor(value);
			case RM_RUP:
				return ceil(value);
			case RM_RMM:
				return round_nearest(value, false);
			case RM_RNE:
			default:
				return round_nearest(value, true);
		}
	}

	static double round_nearest(double value, bool ties_to_even) {
		double lower = floor(value);
		double frac = value - lower;

		if (frac < 0.5) return lower;
		if (frac > 0.5) return lower + 1.0;
		if (!ties_to_even) return value < 0.0 ? lower : lower + 1.0;

		double half = lower / 2.0;
		return half == floor(half) ? lower : lower + 1.0;
	}

	static float round_to_float(double value, uint8_t rm) {
		float nearest = static_cast<float>(value);
		double rounded = static_cast<double>(nearest);

		switch (static_rm(rm)) {
			case RM_RTZ:
				if (value >= 0.0 && rounded > value) return nextafterf(nearest, -INFINITY);
				if (value < 0.0 && rounded < value) return nextafterf(nearest, INFINITY);
				return nearest;
			case RM_RDN:
				return rounded > value ? nextafterf(nearest, -INFINITY) : nearest;
			case RM_RUP:
				return rounded < value ? nextafterf(nearest, INFINITY) : nearest;
			case RM_RMM:
				return round_float_nearest(value, nearest);
			case RM_RNE:
			default:
				return nearest;
		}
	}

	static float round_float_nearest(double value, float nearest) {
		double rounded = static_cast<double>(nearest);
		if (rounded == value) return nearest;

		float lower, upper;
		if (rounded < value) {
			lower = nearest;
			upper = nextafterf(nearest, INFINITY);
		} else {
			lower = nextafterf(nearest, -INFINITY);
			upper = nearest;
		}

		double lower_dist = value - static_cast<double>(lower);
		double upper_dist = static_cast<double>(upper) - value;
		if (lower_dist < upper_dist) return lower;
		if (upper_dist < lower_dist) return upper;
		return value < 0.0 ? lower : upper;
	}

	static uint32_t fcvt_w_s(float value, uint8_t rm) {
		if (isnan(value)) return 0x7fffffffU;
		double rounded = round_to_integral(value, rm);
		if (rounded >= 2147483648.0) return 0x7fffffffU;
		if (rounded < -2147483648.0) return 0x80000000U;
		return static_cast<uint32_t>(static_cast<int32_t>(rounded));
	}

	static uint32_t fcvt_wu_s(float value, uint8_t rm) {
		if (isnan(value)) return 0xffffffffU;
		double rounded = round_to_integral(value, rm);
		if (rounded >= 4294967296.0) return 0xffffffffU;
		if (rounded <= 0.0) return 0;
		return static_cast<uint32_t>(rounded);
	}

	static uint32_t fclass(uint32_t operand) {
		bool sign = (operand & SIGN_MASK) != 0;
		uint32_t exp = operand & EXP_MASK;
		uint32_t frac = operand & FRAC_MASK;

		if (exp == EXP_MASK) {
			if (frac == 0) return sign ? (1U << 0) : (1U << 7);
			return (operand & QUIET_NAN_BIT) ? (1U << 9) : (1U << 8);
		}

		if (exp == 0) {
			if (frac == 0) return sign ? (1U << 3) : (1U << 4);
			return sign ? (1U << 2) : (1U << 5);
		}

		return sign ? (1U << 1) : (1U << 6);
	}
};
