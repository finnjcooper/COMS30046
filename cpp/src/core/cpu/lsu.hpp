#pragma once
#include <cstddef>
#include "exec_path.hpp"
#include "lsq.hpp"
#include "memory.hpp"

class AddressGenerationUnit : public ExecUnit {
public:
	optional<ExecEntry> step() override {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		Value addr = exec(current.op, current.Vj, Value::scalar(current.imm), Value::scalar(0));
		busy_ = false;
		return ExecEntry {current.op, addr, 0, false, current.tag, current.vl, current.sew};
	}

private:
	Value exec(Op op, Value v1, Value v2, Value v3) override {
		uint32_t operand1 = v1.as_scalar(), operand2 = v2.as_scalar();
		return Value::scalar(operand1 + operand2);
	}

	size_t latency(const RSEntry &entry) const override {
		return 1;
	}
};

class LoadStoreUnit {
public:
	optional<uint32_t> step() {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		busy_ = false;
		return current_tag;
	}

	void start(const LSQEntry &entry) {
		current_tag = entry.tag;
		cycles_remaining = cycles(entry.op);
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
	LoadStorePath(size_t agu_count, size_t rs_size, size_t queue_size, Memory &mem) :
		ExecPath(agu_count, rs_size, [] {
			return make_unique<AddressGenerationUnit>();
		}),
		lsus(agu_count),
		lsq(queue_size),
		mem(mem) {}

	bool can_allocate() const override {
		return ExecPath::can_allocate() && lsq.can_allocate();
	}

	void dispatch(const RSEntry &entry) override {
		auto *rs = find_slot();
		if (!rs) throw logic_error("Reservation stations full");
		if (!lsq.can_allocate()) throw logic_error("LSQ full");

		*rs = entry;
		lsq.allocate(entry);
	}

	void issue() override {
		ExecPath::issue();

		for (auto &unit : lsus) {
			if (unit.busy()) continue;

			auto entry = lsq.issue();
			if (!entry) break;
			unit.start(*entry);
		}
	}

	void execute() override {
		ExecPath::execute();

		for (auto &unit : lsus) {
			auto tag = unit.step();
			if (tag) completed.push_back(lsq.complete(*tag, mem));
		}
	}

	void wake(const ExecEntry &exec) override {
		ExecPath::wake(exec);
		lsq.wake(exec);
	}

	void flush(uint32_t tag) override {
		ExecPath::flush(tag);
		lsq.flush(tag);
		for (auto &unit : lsus)
			unit.flush(tag);
	}

	bool commit(uint32_t tag, CommitLog &log) {
		return lsq.commit(tag, mem, log);
	}

protected:
	bool can_issue(const RSEntry &entry) const override {
		return entry.Qj == -1U && entry.Qv == -1U;
	}

	void complete(const ExecEntry &entry) override {
		lsq.update(entry);
	}

private:
	vector<LoadStoreUnit> lsus;
	LoadStoreQueue lsq;
	Memory &mem;
};
