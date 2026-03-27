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
		else if (isStore(op)) return value;
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

public:
	uint32_t store(Op op, uint32_t addr, uint32_t value) {
		switch (op) {
			case SB: {
				uint8_t old_val = mem.loadb(addr), new_val = value & 0xFF;
				mem.storeb(addr, new_val);
				log.recordMemWrite(addr, old_val, new_val, 1);
				break;
			}
			case SH: {
				uint16_t old_val = mem.loadh(addr), new_val = value & 0xFFFF;
				mem.storeh(addr, new_val);
				log.recordMemWrite(addr, old_val, new_val, 2);
				break;
			}
			case SW: {
				uint32_t old_val = mem.loadw(addr);
				mem.storew(addr, value);
				log.recordMemWrite(addr, old_val, value, 4);
				break;
			}
			default:
				break;
		}

		return 0;
	}
};
