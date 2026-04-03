#pragma once
#include <algorithm>
#include <memory>
#include <utility>
#include "exec.hpp"
#include "loadstore.hpp"

class ExecPath {
public:
	~ExecPath() = default;

	template <typename MakeUnit>
	ExecPath(size_t unit_count, size_t rs_size, MakeUnit&& make_unit) : stations(rs_size) {
		units.reserve(unit_count);
		for (size_t i = 0; i < unit_count; i++)
			units.emplace_back(make_unit());
	}

	RSEntry* find_slot() {
		for (auto &rs : stations)
			if (!rs.busy) return &rs;
		return nullptr;
	};

	virtual void issue() {
		for (auto &rs : stations)
			if (rs.busy && rs.Qj == -1U && rs.Qk == -1U)
				for (auto &unit : units) {
					if (unit->busy()) continue;
					unit->start(rs);
					rs.busy = false;
					break;
				}
	};

	virtual void allocate(Op op, uint32_t tag) {}

	void execute() {
		for (auto &unit : units) {
			auto result = unit->step();
			if (result) completed.push_back(result.value());
		}
	};

	vector<ExecEntry> take_finished() {
		vector<ExecEntry> ready;
		ready.swap(completed);
		return ready;
	}

	void wake(uint32_t tag, uint32_t value) {
		for (auto &rs : stations) {
			if (!rs.busy) continue;
			if (rs.Qj == tag) { rs.Vj = value; rs.Qj = -1U; }
			if (rs.Qk == tag) { rs.Vk = value; rs.Qk = -1U; }
		}
	}

	virtual void flush(uint32_t tag) {
		for (auto &rs : stations)
			if (rs.busy && rs.tag > tag) rs.busy = false;
		for (auto &unit : units)
			unit->flush(tag);
		flush_completed(tag);
	};

protected:
	vector<unique_ptr<ExecUnit>> units;
	vector<RSEntry> stations;

	RSEntry* find_slot(uint32_t tag) {
		for (auto &rs : stations)
			if (rs.busy && rs.tag == tag) return &rs;
		return nullptr;
	};

private:
	vector<ExecEntry> completed;

	void flush_completed(uint32_t tag) {
		completed.erase(
			remove_if(completed.begin(), completed.end(), [tag](const ExecEntry &entry) { return entry.tag > tag; }),
			completed.end()
		);
	}
};

class LoadStoreExecPath : public ExecPath {
public:
	template <typename MakeUnit>
	LoadStoreExecPath(size_t unit_count, size_t rs_size, LoadStoreQueue &lsq, MakeUnit&& make_unit) :
		ExecPath(unit_count, rs_size, forward<MakeUnit>(make_unit)), lsq(lsq) {}

	void allocate(Op op, uint32_t tag) override {
		lsq.allocate(op, tag);
	}

	void issue() override {
		for (auto &unit : units) {
			if (unit->busy()) continue;

			bool issued = false;
			for (auto &entry : lsq.get_entries()) {
				if (entry.issued || entry.done) continue;

				RSEntry *rs = find_slot(entry.tag);
				if (!rs || rs->Qj != -1U || rs->Qk != -1U) continue;

				uint32_t addr = rs->Vj + rs->imm;
				if (is_load(entry.op) && !lsq.canIssueLoad(entry.tag, addr)) continue;

				unit->start(*rs);
				rs->busy = false;
				lsq.markIssued(entry.tag);
				issued = true;
				break;
			}

			if (!issued) break;
		}
	}

	void flush(uint32_t tag) override {
		ExecPath::flush(tag);
		lsq.flush(tag);
	}

private:
	LoadStoreQueue &lsq;
};
