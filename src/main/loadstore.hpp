#pragma once
#include "memory.hpp"
#include "decoder.hpp"
#include "exec.hpp"

class LoadStoreUnit : ExecUnit {
public:
	LoadStoreUnit(Memory &memory) : mem(memory) {}

	uint32_t exec(Op op, uint32_t addr, uint32_t value) override {
		if (isLoad(op)) return load(op, addr);
		else if (isStore(op)) return store(op, addr, value);
		return 0;
	}

	uint32_t load(Op op, uint32_t addr) {
		switch (op) {
			case LB:
				return Decoder::sign_extend(mem.loadb(addr), 8);
			case LH:
				return Decoder::sign_extend(mem.loadh(addr), 16);
			case LW:
				return mem.loadw(addr);
			case LBU:
				return mem.loadb(addr);
			case LHU:
				return mem.loadh(addr);
			default:
				return 0;
		}
	}

	uint32_t store(Op op, uint32_t addr, uint32_t value) {
		switch (op) {
			case SB:
				mem.storeb(addr, value & 0xFF);
				break;
			case SH:
				mem.storeh(addr, value & 0xFFFF);
				break;
			case SW:
				mem.storew(addr, value);
				break;
			default:
				break;
		}

		return 0;
	}

private:
	Memory &mem;
};
