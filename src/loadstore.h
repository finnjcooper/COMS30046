#pragma once
#include "memory.h"
#include "instruction.h"
#include "decoder.h"

class LoadStoreUnit {
public:
	uint32_t load(Op op, Memory &mem, uint32_t addr) {
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

	void store(Op op, Memory &mem, uint32_t addr, uint32_t value) {
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
	}
};
