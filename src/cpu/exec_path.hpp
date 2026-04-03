#pragma once
#include <memory>
#include <utility>
#include "exec.hpp"
#include "loadstore.hpp"

class ExecPath {
public:
	~ExecPath() = default;
	ExecPath(vector<unique_ptr<ExecUnit>> units, size_t rs_size, LoadStoreQueue* lsq = nullptr) : units(move(units)), stations(rs_size), lsq(lsq) {}

	RSEntry* find_slot() {
		for (auto &rs : stations)
			if (!rs.busy) return &rs;
		return nullptr;
	};

	void issue() {
		if (lsq != nullptr) { issue_lsu(); return; }

		for (auto &rs : stations)
			if (rs.busy && rs.Qj == -1U && rs.Qk == -1U)
				for (auto &unit : units) {
					if (unit->busy()) continue;
					unit->start(rs);
					rs.busy = false;
					break;
				}
	};

	void issue_lsu() {
		for (auto &unit : units) {
			if (unit->busy()) continue;

			bool issued = false;
			for (auto &entry : lsq->getEntries()) {
				if (entry.issued || entry.done) continue;

				RSEntry* rs_ = find_slot(entry.tag);
				if (!rs_) continue; RSEntry &rs = *rs_;
				if (rs.Qj != -1U || rs.Qk != -1U) continue;

				uint32_t addr = rs.Vj + rs.imm;
				if (isLoad(entry.op) && !lsq->canIssueLoad(entry.tag, addr)) continue;

				unit->start(rs);
				rs.busy = false;
				lsq->markIssued(entry.tag);
				issued = true;
				break;
			}

			if (!issued) break;
		}
	}

	void execute() {
		for (auto &unit : units) {
			auto result = unit->step();
			if (result) completed.push_back(result.value());
		}
	};

	vector<ExecEntry> finished() {
		return completed;
	}

	void consume(uint32_t tag) {
		completed.erase(
			remove_if(completed.begin(), completed.end(), [tag](const ExecEntry &entry) { return entry.tag == tag; }),
			completed.end()
		);
	}

	void wake(uint32_t tag, uint32_t value) {
		for (auto &rs : stations) {
			if (rs.Qj == tag) { rs.Vj = value; rs.Qj = -1U; }
			if (rs.Qk == tag) { rs.Vk = value; rs.Qk = -1U; }
		}
	}

	void flush(uint32_t tag) {
		for (auto &rs : stations)
			if (rs.tag > tag) rs.busy = false;
		for (auto &unit : units)
			unit->flush(tag);
		flush_completed(tag);
	};

private:
	LoadStoreQueue* lsq;
	vector<unique_ptr<ExecUnit>> units;
	vector<RSEntry> stations;
	vector<ExecEntry> completed;

	RSEntry* find_slot(uint32_t tag) {
		for (auto &rs : stations)
			if (rs.tag == tag) return &rs;
		return nullptr;
	};

	void flush_completed(uint32_t tag) {
		completed.erase(
			remove_if(completed.begin(), completed.end(), [tag](const ExecEntry &entry) { return entry.tag > tag; }),
			completed.end()
		);
	}
};
