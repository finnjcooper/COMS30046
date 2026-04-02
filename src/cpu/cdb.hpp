#pragma once
#include <array>
#include "exec.hpp"
#include "rob.hpp"

class CommonDataBus {
public:
	CommonDataBus(size_t lane_count) : lane_count(lane_count) {}

	vector<ExecUnit*> arbitrate(const vector<ExecUnit*> &ready_units) const {
		vector<ExecUnit*> winners = ready_units;

		stable_sort(winners.begin(), winners.end(), [](ExecUnit *lhs, ExecUnit *rhs) {
			return lhs->peekResult().tag < rhs->peekResult().tag;
		});

		if (winners.size() > lane_count) winners.resize(lane_count);
		return winners;
	}

	void broadcast(const ExecEntry &exec, ReOrderBuffer &rob, const array<vector<RSEntry>*, 4> &rs_banks) const {
		rob.set(exec.tag, exec.value, exec.addr, exec.jumped, exec.should_halt);

		for (auto *rs_bank : rs_banks)
			for (auto &rs : *rs_bank)
				update(rs, exec.tag, exec.value);
	}

private:
	size_t lane_count;

	static void update(RSEntry &rs, uint32_t tag, uint32_t value) {
		if (rs.Qj == tag) {
			rs.Vj = value;
			rs.Qj = -1U;
		}

		if (rs.Qk == tag) {
			rs.Vk = value;
			rs.Qk = -1U;
		}
	}
};
