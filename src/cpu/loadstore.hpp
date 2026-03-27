#pragma once
#include "memory.hpp"
#include "trace.hpp"
#include "exec.hpp"

class LoadStoreUnit : public ExecUnit {
public:
	LoadStoreUnit(Memory &memory, CommitLog &log) : mem(memory), log(log) { cycles = 1UL; }

	void step() override {
		if (!busy_) return;
		if (--cycles_remaining == 0) {
			uint32_t addr = current.Vj + current.imm;
			uint32_t ls_out = exec(current.op, addr, current.Vk);
			
			result = {current.op, ls_out, addr, 0, false, false, current.tag};

			busy_ = false; done_ = true;
		}
	}

	uint32_t exec(Op op, uint32_t addr, uint32_t value) override {
		if (isLoad(op)) return load(op, addr);
		else if (isStore(op)) value;
		return 0;
	}

private:
	Memory &mem;
	CommitLog &log;

	uint32_t load(Op op, uint32_t addr) {
		switch (op) {
			case LB:
				return signExtend(mem.loadb(addr), 8);
			case LH:
				return signExtend(mem.loadh(addr), 16);
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
};
