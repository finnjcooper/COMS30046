#pragma once
#include <cmath>
#include <cstring>
#include <limits>
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
			uint32_t avl = current.op == VSETIVLI
				? static_cast<uint32_t>(current.imm)
				: (current.imm == -1 ? state.vector_bits / sew : current.Vj.as_scalar());
			uint8_t vl = state.vl_for(avl, sew);
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
			case VADD_VV: return binary_vv([](uint32_t a, uint32_t b) { return a + b; });
			case VADD_VX: return binary_vx([](uint32_t a, uint32_t b) { return a + b; });
			case VADD_VI: return binary_vi([](uint32_t a, uint32_t b) { return a + b; });
			case VMUL_VV: return binary_vv([](uint32_t a, uint32_t b) { return a * b; });
			case VMUL_VX: return binary_vx([](uint32_t a, uint32_t b) { return a * b; });
			case VMACC_VV: return vmacc();
			case VMADD_VV: return vmadd();
			case VMV_V_I: return vmv_v_i();
			case VMV_S_X: return vmv_s_x();
			case VMV_X_S: return Value::scalar(vmv_x_s());
			case VREDSUM_VS: return vredsum();
			case VFMV_V_F: return vfmv_v_f();
			case VFMACC_VV: return vfmac(true);
			case VFMADD_VV: return vfmac(false);
			default: return Value::scalar(0);
		}
	}

	template <typename Fn>
	Value binary_vv(Fn fn) {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(fn(current.Vj.lane(lane), current.Vk.lane(lane))));
		return result;
	}

	template <typename Fn>
	Value binary_vx(Fn fn) {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		uint32_t scalar = current.Vk.as_scalar();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(fn(current.Vj.lane(lane), scalar)));
		return result;
	}

	template <typename Fn>
	Value binary_vi(Fn fn) {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		uint32_t imm = static_cast<uint32_t>(current.imm);
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(fn(current.Vj.lane(lane), imm)));
		return result;
	}

	Value vmacc() {
		Value result = current.Vj;
		result.vector_value = true;
		for (uint8_t lane = 0; lane < current.vl; lane++) {
			uint32_t value = current.Vj.lane(lane) + current.Vk.lane(lane) * current.Vl.lane(lane);
			result.set_lane(lane, wrap(value));
		}
		return result;
	}

	Value vmadd() {
		Value result = current.Vj;
		result.vector_value = true;
		for (uint8_t lane = 0; lane < current.vl; lane++) {
			uint32_t value = current.Vk.lane(lane) * current.Vj.lane(lane) + current.Vl.lane(lane);
			result.set_lane(lane, wrap(value));
		}
		return result;
	}

	Value vmv_v_i() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, wrap(static_cast<uint32_t>(current.imm)));
		return result;
	}

	Value vmv_s_x() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		result.set_lane(0, wrap(current.Vj.as_scalar()));
		return result;
	}

	uint32_t vmv_x_s() {
		uint32_t lane = current.Vj.lane(0);
		switch (current.sew) {
			case 8: return sign_extend(lane & 0xFF, 8);
			case 16: return sign_extend(lane & 0xFFFF, 16);
			default: return lane;
		}
	}

	Value vredsum() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		uint32_t sum = current.Vk.lane(0);
		for (uint8_t lane = 0; lane < current.vl; lane++)
			sum += current.Vj.lane(lane);
		result.set_lane(0, wrap(sum));
		return result;
	}

	Value vfmv_v_f() {
		Value result = current.Vl.is_vector() ? current.Vl : Value::vector_zero();
		if (current.sew != 32) return result;
		uint32_t bits = current.Vj.as_scalar();
		for (uint8_t lane = 0; lane < current.vl; lane++)
			result.set_lane(lane, bits);
		return result;
	}

	Value vfmac(bool accumulate) {
		Value result = current.Vj;
		result.vector_value = true;
		if (current.sew != 32) return result;

		for (uint8_t lane = 0; lane < current.vl; lane++) {
			float dest = bits_to_float(current.Vj.lane(lane));
			float lhs = bits_to_float(current.Vk.lane(lane));
			float rhs = bits_to_float(current.Vl.lane(lane));
			float value = accumulate ? fma(lhs, rhs, dest) : fma(lhs, dest, rhs);
			result.set_lane(lane, float_to_bits(value));
		}
		return result;
	}

	uint32_t wrap(uint32_t value) const {
		switch (current.sew) {
			case 8: return value & 0xFF;
			case 16: return value & 0xFFFF;
			default: return value;
		}
	}

	static float bits_to_float(uint32_t bits) {
		float value;
		memcpy(&value, &bits, sizeof(value));
		return value;
	}

	static uint32_t float_to_bits(float value) {
		uint32_t bits;
		memcpy(&bits, &value, sizeof(bits));
		return bits;
	}
};
