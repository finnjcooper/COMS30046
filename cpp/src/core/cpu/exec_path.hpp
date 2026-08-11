#pragma once
#include <algorithm>
#include <memory>
#include <utility>
#include "exec.hpp"

struct ExecPathSnapshot {
	vector<ExecSnapshot> units;
	vector<RSEntry> stations;
	// completed?
};

class ExecPath {
public:
	virtual ~ExecPath() = default;

	template <typename MakeUnit>
	ExecPath(size_t unit_count, size_t rs_size, MakeUnit&& make_unit) : stations(rs_size) {
		units.reserve(unit_count);
		for (size_t i = 0; i < unit_count; i++)
			units.emplace_back(make_unit());
	}

	virtual void clear() {
		for (auto &unit : units) unit->clear();
		fill(stations.begin(), stations.end(), RSEntry());
		completed.clear();
	}

	ExecPathSnapshot snapshot() const {
		ExecPathSnapshot s;
		for (const auto &unit : units)
			s.units.push_back(unit->snapshot());
		s.stations = stations;
		return s;
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
	}

	virtual void issue() {
		for (auto &rs : stations)
			if (rs.busy && can_issue(rs))
				for (auto &unit : units) {
					if (unit->busy()) continue;
					unit->start(rs);
					rs.busy = false;
					break;
				}
	};

	virtual void execute() {
		for (auto &unit : units) {
			auto result = unit->step();
			if (result) complete(result.value());
		}
	};

	vector<ExecEntry> take_finished() {
		vector<ExecEntry> ready;
		ready.swap(completed);
		return ready;
	}

	virtual void wake(const ExecEntry &exec) {
		for (auto &rs : stations) {
			if (!rs.busy) continue;
			if (rs.Qj == exec.tag) { rs.Vj = exec.value; rs.Qj = -1U; }
			if (rs.Qk == exec.tag) { rs.Vk = exec.value; rs.Qk = -1U; }
			if (rs.Ql == exec.tag) { rs.Vl = exec.value; rs.Ql = -1U; }
			if (rs.Qv == exec.tag) {
				rs.vl = exec.vl;
				rs.sew = exec.sew;
				rs.Qv = -1U;
			}
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

	virtual bool can_issue(const RSEntry &entry) const {
		return entry.Qj == -1U && entry.Qk == -1U && entry.Ql == -1U && entry.Qv == -1U;
	}

	virtual void complete(const ExecEntry &entry) {
		completed.push_back(entry);
	}

	void flush_completed(uint32_t tag) {
		completed.erase(
			remove_if(completed.begin(), completed.end(), [tag](const ExecEntry &entry) { return entry.tag > tag; }),
			completed.end()
		);
	}
};
