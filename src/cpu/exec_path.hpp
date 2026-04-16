#pragma once
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include "exec.hpp"

class ExecPath {
public:
	virtual ~ExecPath() = default;

	template <typename MakeUnit>
	ExecPath(size_t unit_count, size_t rs_size, MakeUnit&& make_unit) : stations(rs_size) {
		units.reserve(unit_count);
		for (size_t i = 0; i < unit_count; i++)
			units.emplace_back(make_unit());
	}

	virtual bool can_allocate() const {
		for (const auto &rs : stations)
			if (!rs.busy) return true;
		return false;
	}

	virtual RSEntry* find_slot() {
		for (auto &rs : stations)
			if (!rs.busy) return &rs;
		return nullptr;
	};

	virtual void dispatch(const RSEntry &entry) {
		auto *rs = find_slot();
		if (!rs) throw logic_error("Reservation stations full");

		*rs = entry;
		allocate(entry.op, entry.tag);
	}

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

	virtual void execute() {
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

	virtual void wake(uint32_t tag, uint32_t value) {
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
	ExecPath() = default;

	vector<unique_ptr<ExecUnit>> units;
	vector<RSEntry> stations;
	vector<ExecEntry> completed;

	void flush_completed(uint32_t tag) {
		completed.erase(
			remove_if(completed.begin(), completed.end(), [tag](const ExecEntry &entry) { return entry.tag > tag; }),
			completed.end()
		);
	}
};
