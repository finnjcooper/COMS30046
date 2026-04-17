#pragma once
#include <cstddef>
#include "exec_path.hpp"
#include "lsq.hpp"
#include "memory.hpp"

class LoadStoreUnit {
public:
	optional<uint32_t> step() {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		busy_ = false;
		return current_tag;
	}

	void start(uint32_t tag) {
		current_tag = tag;
		cycles_remaining = cycles(LB);
		busy_ = true;
	}

	bool busy() const { return busy_; }

	void flush(uint32_t tag) {
		if (busy_ && current_tag > tag) busy_ = false;
	}

private:
	size_t cycles_remaining = 0;
	uint32_t current_tag = -1U;
	bool busy_ = false;
};

class LoadStorePath : public ExecPath {
public:
	LoadStorePath(size_t unit_count, size_t queue_size, Memory &mem) :
		lsus(unit_count), lsq(queue_size), mem(mem) {}

	bool can_allocate() const override {
		return lsq.can_allocate();
	}

	void dispatch(const RSEntry &entry) override {
		lsq.allocate(entry.op, entry.tag, entry.Vj, entry.Vk, entry.Qj, entry.Qk, entry.imm);
	}

	void issue() override {
		for (auto &unit : lsus) {
			if (unit.busy()) continue;

			auto tag = lsq.issue();
			if (!tag) break;
			unit.start(*tag);
		}
	}

	void execute() override {
		for (auto &unit : lsus) {
			auto tag = unit.step();
			if (tag) completed.push_back(lsq.complete(*tag, mem));
		}
	}

	void wake(uint32_t tag, uint32_t value) override {
		lsq.wake(tag, value);
	}

	void flush(uint32_t tag) override {
		lsq.flush(tag);
		for (auto &unit : lsus)
			unit.flush(tag);
		flush_completed(tag);
	}

	bool commit(uint32_t tag, CommitLog &log) {
		return lsq.commit(tag, mem, log);
	}

private:
	vector<LoadStoreUnit> lsus;
	LoadStoreQueue lsq;
	Memory &mem;
};
