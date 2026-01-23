#pragma once
#include "memory.h"
#include "instruction.h"
#include "decoder.h"
#include <cstdint>

class LoadStoreUnit {
public:
	uint32_t load(Opcode op, Memory& mem, uint32_t addr) {
		switch (op) {
			case Opcode::LB:
				return Decoder::sign_extend(mem.loadb(addr), 8);
			case Opcode::LH:
				return Decoder::sign_extend(mem.loadh(addr), 16);
			case Opcode::LW:
				return mem.loadw(addr);
			case Opcode::LBU:
				return mem.loadb(addr);
			case Opcode::LHU:
				return mem.loadh(addr);
			default:
				return 0;
		}
	}

	void store(Opcode op, Memory& mem, uint32_t addr, uint32_t value) {
		switch (op) {
			case Opcode::SB:
				mem.storeb(addr, value & 0xFF);
				break;
			case Opcode::SH:
				mem.storeh(addr, value & 0xFFFF);
				break;
			case Opcode::SW:
				mem.storew(addr, value);
				break;
			default:
				break;
		}
	}
};
