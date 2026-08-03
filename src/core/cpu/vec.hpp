#pragma once
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include "exec.hpp"

class VectorUnit : public ExecUnit {
public:
	VectorUnit(VectorState &state) : state(state) {}

	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		ExecEntry entry;
		entry.op = current.op;
		entry.tag = current.tag;
		entry.vl = current.vl;
		entry.sew = current.sew;

		if (is_vset(current.op)) {
			uint8_t sew = current.sew;
			bool keep_vl = current.op == VSETVLI && current.imm == -2;
			uint32_t avl = current.op == VSETIVLI
				? static_cast<uint32_t>(current.imm)
				: (current.imm == -1 ? state.vector_bits / sew : current.Vj.as_scalar());
			uint8_t vl = keep_vl ? state.vl : state.vl_for(avl, sew);
			entry.value = Value::scalar(vl);
			entry.vl = vl;
			entry.sew = sew;
			busy_ = false;
			return entry;
		}

		entry.value = exec(current.op, current.Vj, current.Vk, current.Vl);
		busy_ = false;
		return entry;
	}

private:
	VectorState &state;

	Value exec(Op op, Value v1, Value v2, Value v3) override {
		switch (op) {
			case VMUL_VV: return binary_vv([](uint32_t a, uint32_t b) { return a * b; });
			case VMACC_VV: return vmacc_vv();
			case VMACC_VX: return vmacc_vx();
			case VMV_V_I: return vmv_v_i();
			case VMV_V_X: return vmv_v_x();
			case VMV_S_X: return vmv_s_x();
			case VMV_X_S: return Value::scalar(vmv_x_s());
			case VMV1R_V: return vmv1r_v();
			case VREDSUM_VS: return vredsum();
			case VFMV_V_F: return vfmv_v_f();
			case VFMACC_VV: return vfmac_vv();
			case VFMACC_VF: return vfmac_vf();
			default: throw invalid_argument("Unsupported vector operation");
		}
	}

	template <typename Fn>
	Value binary_vv(Fn fn) {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(current.sew, fn(current.Vj.lane(lane), current.Vk.lane(lane))));
		return result;
	}

	Value vmv_v_i() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(current.sew, static_cast<uint32_t>(current.imm)));
		return result;
	}

	Value vmv_v_x() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		uint32_t scalar = current.Vj.as_scalar();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(current.sew, scalar));
		return result;
	}

	uint32_t vmv_x_s() {
		return static_cast<uint32_t>(sign_extend_lane(current.sew, current.Vj.lane(0)));
	}

	Value vmv_s_x() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		result.set_lane(0, wrap(current.sew, current.Vj.as_scalar()));
		return result;
	}

	Value vmv1r_v() {
		Value result = current.Vj;
		result.vector_value = true;
		return result;
	}

	Value vmacc_vv() {
		Value result = current.Vj;
		result.vector_value = true;
		for (uint8_t lane = 0; lane < current.vl; lane++) {
			uint32_t acc = current.Vj.lane(lane);
			uint32_t lhs = current.Vk.lane(lane);
			uint32_t rhs = current.Vl.lane(lane);
			result.set_lane(lane, wrap(current.sew, acc + lhs * rhs));
		}
		return result;
	}

	Value vmacc_vx() {
		Value result = current.Vj;
		result.vector_value = true;
		uint32_t scalar = current.Vk.as_scalar();
		for (uint8_t lane = 0; lane < current.vl; lane++) {
			uint32_t acc = current.Vj.lane(lane);
			uint32_t vec = current.Vl.lane(lane);
			result.set_lane(lane, wrap(current.sew, acc + scalar * vec));
		}
		return result;
	}

	Value vredsum() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		uint32_t sum = wrap(current.sew, current.Vk.lane(0));
		for (uint8_t lane = 0; lane < current.vl; lane++)
			sum = wrap(current.sew, sum + current.Vj.lane(lane));
		result.set_lane(0, sum);
		return result;
	}

	Value vfmv_v_f() {
		require_float_sew();
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		uint32_t bits = current.Vj.as_scalar();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, bits);
		return result;
	}

	Value vfmac_vv() {
		require_float_sew();
		Value result = current.Vj;
		result.vector_value = true;
		for (uint8_t lane = 0; lane < current.vl; lane++) {
			float dest = bits_to_float(current.Vj.lane(lane));
			float lhs = bits_to_float(current.Vk.lane(lane));
			float rhs = bits_to_float(current.Vl.lane(lane));
			result.set_lane(lane, float_to_bits(fma(lhs, rhs, dest)));
		}
		return result;
	}

	Value vfmac_vf() {
		require_float_sew();
		Value result = current.Vj;
		result.vector_value = true;
		float scalar = bits_to_float(current.Vk.as_scalar());
		for (uint8_t lane = 0; lane < current.vl; lane++) {
			float dest = bits_to_float(current.Vj.lane(lane));
			float vec = bits_to_float(current.Vl.lane(lane));
			result.set_lane(lane, float_to_bits(fma(scalar, vec, dest)));
		}
		return result;
	}

	static uint32_t lane_mask(uint8_t sew) {
		switch (sew) {
			case 8: return 0xFFU;
			case 16: return 0xFFFFU;
			case 32: return 0xFFFFFFFFU;
			default: throw invalid_argument("Unsupported vector SEW");
		}
	}

	static uint32_t wrap(uint8_t sew, uint32_t value) {
		return value & lane_mask(sew);
	}

	static int32_t sign_extend_lane(uint8_t sew, uint32_t value) {
		switch (sew) {
			case 8: return sign_extend(value & 0xFF, 8);
			case 16: return sign_extend(value & 0xFFFF, 16);
			case 32: return static_cast<int32_t>(value);
			default: throw invalid_argument("Unsupported vector SEW");
		}
	}

	void require_float_sew() const {
		if (current.sew != 32) throw invalid_argument("Vector float operation requires SEW=32");
	}

	static bool is_nan(uint32_t bits) {
		return (bits & EXP_MASK) == EXP_MASK && (bits & FRAC_MASK) != 0;
	}

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

	static constexpr uint32_t EXP_MASK = 0x7f800000U;
	static constexpr uint32_t FRAC_MASK = 0x007fffffU;
	static constexpr uint32_t CANONICAL_NAN = 0x7fc00000U;
};
