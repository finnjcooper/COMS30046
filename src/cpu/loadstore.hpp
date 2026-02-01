#pragma once
#include "memory.hpp"
#include "exec.hpp"

class LoadStoreUnit : ExecUnit {
public:
	LoadStoreUnit(Memory &memory, CommitLog &log) : mem(memory), log(log) {}

	uint32_t exec(Op op, uint32_t addr, uint32_t value) override {
		if (isLoad(op)) return load(op, addr);
		else if (isStore(op)) return store(op, addr, value);
		return 0;
	}

	uint32_t load(Op op, uint32_t addr) {
		switch (op) {
			case LB:
				return sign_extend(mem.loadb(addr), 8);
			case LH:
				return sign_extend(mem.loadh(addr), 16);
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
				log.recordMemWrite(addr, mem.loadb(addr), value & 0xFF, 1);
				break;
			case SH:
				mem.storeh(addr, value & 0xFFFF);
				log.recordMemWrite(addr, mem.loadh(addr), value & 0xFFFF, 2);
				break;
			case SW:
				mem.storew(addr, value);
				log.recordMemWrite(addr, mem.loadw(addr), value, 4);
				break;
			default:
				break;
		}

		return 0;
	}

private:
	Memory &mem;
	CommitLog &log;
};
