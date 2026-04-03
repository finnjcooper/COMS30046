#pragma once
#include <algorithm>
#include <deque>
#include <optional>
#include <stdexcept>
#include "instruction.hpp"
#include "memory.hpp"
#include "pipeline.hpp"
#include "trace.hpp"

struct LSQEntry {
	Op op = INVALID;
	uint32_t tag = -1U;

	uint32_t Vj = 0, Vk = 0;
	uint32_t Qj = -1U, Qk = -1U;

	int32_t imm = 0;
	uint32_t addr = 0;

	bool issued = false;
	bool done = false;
};

class LoadStoreQueue {
public:
	LoadStoreQueue(size_t size) : max_size(size) {}

	bool can_allocate() const {
		return entries.size() < max_size;
	}

	void allocate(Op op, uint32_t tag, uint32_t Vj, uint32_t Vk, uint32_t Qj, uint32_t Qk, int32_t imm) {
		uint32_t addr = 0;
		if (Qj == -1U) addr = Vj + imm;

		entries.push_back(LSQEntry {op, tag, Vj, Vk, Qj, Qk, imm, addr, false, false});
	}

	const deque<LSQEntry>& get_entries() const { return entries; }

	optional<uint32_t> issue() {
		for (auto &entry : entries) {
			if (!ready_to_issue(entry)) continue;
			entry.issued = true;
			return entry.tag;
		}

		return nullopt;
	}

	ExecEntry complete(uint32_t tag, const Memory &mem) {
		size_t idx = index_of(tag);
		auto &entry = entries[idx];

		if (is_load(entry.op)) {
			uint32_t raw = load(entry, mem);
			entry.Vk = format_load(entry.op, raw);
		}

		entry.issued = false;
		entry.done = true;
		return ExecEntry {entry.op, entry.Vk, entry.addr, 0, false, false, entry.tag};
	}

	void commit(uint32_t tag, Memory &mem, CommitLog &log) {
		if (entries.empty()) return;

		const auto entry = entries.front();
		if (!entry.done) return;
		if (entry.tag != tag) throw logic_error("LSQ head tag does not match commit tag");

		if (is_store(entry.op)) store(entry, mem, log);
		entries.pop_front();
	}

	void wake(uint32_t tag, uint32_t value) {
		for (auto &entry : entries) {
			if (entry.done) continue;

			if (entry.Qj == tag) {
				entry.Qj = -1U;
				entry.Vj = value;
				update_addr(entry);
			}

			if (is_store(entry.op) && entry.Qk == tag) {
				entry.Qk = -1U;
				entry.Vk = value;
			}
		}
	}

	void flush(uint32_t tag) {
		entries.erase(
			remove_if(entries.begin(), entries.end(), [tag](const LSQEntry &entry) { return entry.tag > tag; }),
			entries.end()
		);
	}

private:
	size_t max_size;
	deque<LSQEntry> entries;

	static uint8_t access_size(Op op) {
		switch (op) {
			case LB: case LBU: case SB:
				return 1;
			case LH: case LHU: case SH:
				return 2;
			case LW: case SW:
				return 4;
			default:
				return 0;
		}
	}

	static uint32_t format_load(Op op, uint32_t raw) {
		switch (op) {
			case LB:
				return sign_extend(raw & 0xFF, 8);
			case LH:
				return sign_extend(raw & 0xFFFF, 16);
			case LW:
				return raw;
			case LBU:
				return raw & 0xFF;
			case LHU:
				return raw & 0xFFFF;
			default:
				return 0;
		}
	}

	static void update_addr(LSQEntry &entry) {
		entry.addr = entry.Vj + entry.imm;
	}

	static uint32_t load(const LSQEntry &entry, const Memory &mem) {
		switch (entry.op) {
			case LB: case LBU:
				return mem.loadb(entry.addr);
			case LH: case LHU:
				return mem.loadh(entry.addr);
			case LW:
				return mem.loadw(entry.addr);
			default:
				return 0;
		}
	}


	static void store(const LSQEntry &entry, Memory &mem, CommitLog &log) {
		switch (entry.op) {
			case SB: {
				uint8_t old_val = mem.loadb(entry.addr), new_val = entry.Vk & 0xFF;
				mem.storeb(entry.addr, new_val);
				log.record_mem_write(entry.addr, old_val, new_val, 1);
				break;
			}
			case SH: {
				uint16_t old_val = mem.loadh(entry.addr), new_val = entry.Vk & 0xFFFF;
				mem.storeh(entry.addr, new_val);
				log.record_mem_write(entry.addr, old_val, new_val, 2);
				break;
			}
			case SW: {
				uint32_t old_val = mem.loadw(entry.addr);
				mem.storew(entry.addr, entry.Vk);
				log.record_mem_write(entry.addr, old_val, entry.Vk, 4);
				break;
			}
			default:
				break;
		}
	}

	bool ready_to_issue(const LSQEntry &entry) {
		if (entry.issued || entry.done || entry.Qj != -1U) return false;
		if (is_store(entry.op) && entry.Qk != -1U) return false;
		if (is_load(entry.op) && !can_issue_load(entry)) return false;
		return true;
	}

	bool can_issue_load(const LSQEntry &load_entry) const {
		size_t load_idx = index_of(load_entry.tag);

		for (size_t i = 0; i < load_idx; i++) {
			const auto &entry = entries[i];
			if (!is_store(entry.op)) continue;
			return false;
		}

		return true;
	}

	size_t index_of(uint32_t tag) const {
		for (size_t i = 0; i < entries.size(); i++)
			if (entries[i].tag == tag) return i;

		throw out_of_range("LSQ tag not found");
	}
};
