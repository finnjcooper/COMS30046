#pragma once
#include <algorithm>
#include <deque>
#include <optional>
#include "exec.hpp"
#include "helpers.hpp"
#include "memory.hpp"
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
			if (!can_issue(entry)) continue;
			entry.issued = true;
			return entry.tag;
		}

		return nullopt;
	}

	ExecEntry complete(uint32_t tag, const Memory &mem) {
		size_t idx = index_of(tag);
		auto &entry = entries[idx];

		if (is_load(entry.op)) {
			uint32_t raw = load_forwarded(entry, idx, mem);
			entry.Vk = format_load(entry.op, raw);
		}

		entry.issued = false;
		entry.done = true;
		return ExecEntry {entry.op, entry.Vk, entry.addr, 0, false, false, entry.tag};
	}

	bool commit(uint32_t tag, Memory &mem, CommitLog &log) {
		if (entries.empty()) return false;

		const auto entry = entries.front();
		if (entry.tag != tag) throw logic_error("LSQ head tag does not match commit tag");
		if (!entry.done) return false;

		if (is_store(entry.op)) store(entry, mem, log);
		entries.pop_front();
		return true;
	}

	void wake(uint32_t tag, uint32_t value) {
		for (auto &entry : entries) {
			if (entry.done) continue;

			if (entry.Qj == tag) {
				entry.Qj = -1U;
				entry.Vj = value;
				entry.addr = value + entry.imm;
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
			case LW: case SW: case FLW: case FSW:
				return 4;
			default:
				return 0;
		}
	}

	static uint32_t format_load(Op op, uint32_t raw) {
		switch (op) {
			case LB: return sign_extend(raw & 0xFF, 8);
			case LH: return sign_extend(raw & 0xFFFF, 16);
			case LW:
			case FLW: return raw;
			case LBU: return raw & 0xFF;
			case LHU: return raw & 0xFFFF;
			default: return 0;
		}
	}

	static uint32_t load(const LSQEntry &entry, const Memory &mem) {
		switch (entry.op) {
			case LB: case LBU: return mem.loadb(entry.addr);
			case LH: case LHU: return mem.loadh(entry.addr);
			case LW: case FLW: return mem.loadw(entry.addr);
			default: return 0;
		}
	}

	uint32_t load_forwarded(const LSQEntry &load_entry, size_t load_idx, const Memory &mem) const {
		uint32_t raw = load(load_entry, mem);
		uint8_t load_size = access_size(load_entry.op);

		for (size_t i = 0; i < load_idx; i++) {
			const auto &store_entry = entries[i];
			if (!is_store(store_entry.op) || !overlaps(store_entry, load_entry)) continue;

			uint8_t store_size = access_size(store_entry.op);
			for (uint8_t load_byte = 0; load_byte < load_size; load_byte++) {
				uint64_t byte_addr = static_cast<uint64_t>(load_entry.addr) + load_byte;
				uint64_t store_start = store_entry.addr;
				uint64_t store_end = store_start + store_size;
				if (byte_addr < store_start || byte_addr >= store_end) continue;

				uint8_t store_byte = static_cast<uint8_t>(byte_addr - store_start);
				uint32_t shift = load_byte * 8;
				uint32_t forwarded = ((store_entry.Vk >> (store_byte * 8)) & 0xFF) << shift;
				raw = (raw & ~(0xFFU << shift)) | forwarded;
			}
		}

		return raw;
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
			case SW:
			case FSW: {
				uint32_t old_val = mem.loadw(entry.addr);
				mem.storew(entry.addr, entry.Vk);
				log.record_mem_write(entry.addr, old_val, entry.Vk, 4);
				break;
			}
			default:
				break;
		}
	}

	bool can_issue(const LSQEntry &entry) const {
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
			if (entry.Qj != -1U) return false;
			if (overlaps(entry, load_entry) && entry.Qk != -1U) return false;
		}

		return true;
	}

	static bool overlaps(const LSQEntry &a, const LSQEntry &b) {
		uint64_t a_start = a.addr;
		uint64_t b_start = b.addr;
		uint64_t a_end = a_start + access_size(a.op);
		uint64_t b_end = b_start + access_size(b.op);

		return a_start < b_end && b_start < a_end;
	}

	size_t index_of(uint32_t tag) const {
		for (size_t i = 0; i < entries.size(); i++)
			if (entries[i].tag == tag) return i;

		throw out_of_range("LSQ tag not found");
	}
};
