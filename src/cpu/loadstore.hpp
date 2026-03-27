#pragma once
#include "memory.hpp"
#include "lsq.hpp"
#include "exec.hpp"

class LoadStoreUnit : public ExecUnit {
public:
	LoadStoreUnit(Memory &memory, LoadStoreQueue &lsq) : mem(memory), lsq(lsq) { cycles = 1UL; }

	void step() override {
		if (!busy_) return;
		if (--cycles_remaining == 0) {
			uint32_t addr = current.Vj + current.imm;
			uint32_t ls_out = exec(current.op, addr, current.Vk);
			
			result = {current.op, ls_out, addr, 0, false, false, current.tag};

			busy_ = false; done_ = true;
		}
	}

private:
	Memory &mem;
	LoadStoreQueue &lsq;

	uint32_t exec(Op op, uint32_t addr, uint32_t value) override {
		if (isLoad(op)) return lsq.completeLoad(current.tag, addr, mem);
		if (isStore(op)) lsq.completeStore(current.tag, addr, value);
		return 0;
	}
};
