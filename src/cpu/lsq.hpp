#pragma once
#include <algorithm>
#include <deque>
#include <stdexcept>
#include "instruction.hpp"
#include "memory.hpp"
#include "trace.hpp"

struct LSQEntry {
	Op op = INVALID;
	uint32_t tag = -1U;

	uint32_t addr = 0;
	uint32_t value = 0;

	bool addr_ready = false;
	bool value_ready = false;
	bool issued = false;
	bool done = false;
};

class LoadStoreQueue {
public:
	void allocate(Op op, uint32_t tag) {
		entries.push_back({op, tag});
	}

	const deque<LSQEntry>& get_entries() const { return entries; }

	void markIssued(uint32_t tag) {
		get(tag).issued = true;
	}

	bool canIssueLoad(uint32_t tag, uint32_t addr) const {
		size_t load_idx = indexOf(tag);
		uint8_t load_size = accessSize(entries[load_idx].op);

		for (size_t i = 0; i < load_idx; i++) {
			const auto &entry = entries[i];
			if (!is_store(entry.op)) continue;
			if (!entry.addr_ready) return false;
			if (overlaps(addr, load_size, entry.addr, accessSize(entry.op)) && !entry.value_ready)
				return false;
		}

		return true;
	}

	uint32_t completeLoad(uint32_t tag, uint32_t addr, const Memory &mem) {
		size_t load_idx = indexOf(tag);
		auto &entry = entries[load_idx];
		uint8_t load_size = accessSize(entry.op);
		uint32_t raw = 0;

		entry.addr = addr;
		entry.addr_ready = true;

		for (uint8_t i = 0; i < load_size; i++)
			raw |= static_cast<uint32_t>(loadByte(load_idx, addr + i, mem)) << (i * 8);

		entry.value = formatLoad(entry.op, raw);
		entry.value_ready = true;
		entry.issued = false;
		entry.done = true;

		return entry.value;
	}

	void completeStore(uint32_t tag, uint32_t addr, uint32_t value) {
		auto &entry = get(tag);
		entry.addr = addr;
		entry.value = maskStoreValue(entry.op, value);
		entry.addr_ready = true;
		entry.value_ready = true;
		entry.issued = false;
		entry.done = true;
	}

	void commit(uint32_t tag, Memory &mem, CommitLog &log) {
		if (entries.empty() || entries.front().tag != tag)
			throw logic_error("LSQ commit order mismatch");

		const auto entry = entries.front();
		if (is_store(entry.op)) applyStore(entry, mem, log);
		entries.pop_front();
	}

	void flush(uint32_t tag) {
		entries.erase(
			remove_if(entries.begin(), entries.end(), [tag](const LSQEntry &entry) { return entry.tag > tag; }),
			entries.end()
		);
	}

private:
	deque<LSQEntry> entries;

	static uint8_t accessSize(Op op) {
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

	static uint32_t maskStoreValue(Op op, uint32_t value) {
		switch (op) {
			case SB:
				return value & 0xFF;
			case SH:
				return value & 0xFFFF;
			default:
				return value;
		}
	}

	static uint32_t formatLoad(Op op, uint32_t raw) {
		switch (op) {
			case LB:
				return signExtend(raw & 0xFF, 8);
			case LH:
				return signExtend(raw & 0xFFFF, 16);
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

	static bool overlaps(uint32_t lhs_addr, uint8_t lhs_size, uint32_t rhs_addr, uint8_t rhs_size) {
		return lhs_addr < rhs_addr + rhs_size && rhs_addr < lhs_addr + lhs_size;
	}

	static bool coversByte(const LSQEntry &entry, uint32_t addr) {
		uint8_t size = accessSize(entry.op);
		return entry.addr_ready && addr >= entry.addr && addr < entry.addr + size;
	}

	static uint8_t storeByte(const LSQEntry &entry, uint32_t addr) {
		uint32_t offset = addr - entry.addr;
		return (entry.value >> (offset * 8)) & 0xFF;
	}

	static void applyStore(const LSQEntry &entry, Memory &mem, CommitLog &log) {
		switch (entry.op) {
			case SB: {
				uint8_t old_val = mem.loadb(entry.addr), new_val = entry.value & 0xFF;
				mem.storeb(entry.addr, new_val);
				log.recordMemWrite(entry.addr, old_val, new_val, 1);
				break;
			}
			case SH: {
				uint16_t old_val = mem.loadh(entry.addr), new_val = entry.value & 0xFFFF;
				mem.storeh(entry.addr, new_val);
				log.recordMemWrite(entry.addr, old_val, new_val, 2);
				break;
			}
			case SW: {
				uint32_t old_val = mem.loadw(entry.addr);
				mem.storew(entry.addr, entry.value);
				log.recordMemWrite(entry.addr, old_val, entry.value, 4);
				break;
			}
			default:
				break;
		}
	}

	uint8_t loadByte(size_t load_idx, uint32_t addr, const Memory &mem) const {
		for (size_t i = load_idx; i-- > 0;) {
			const auto &entry = entries[i];
			if (!is_store(entry.op) || !coversByte(entry, addr) || !entry.value_ready) continue;
			return storeByte(entry, addr);
		}

		return mem.loadb(addr);
	}

	size_t indexOf(uint32_t tag) const {
		for (size_t i = 0; i < entries.size(); i++)
			if (entries[i].tag == tag) return i;

		throw out_of_range("LSQ tag not found");
	}

	LSQEntry& get(uint32_t tag) {
		for (auto &entry : entries)
			if (entry.tag == tag) return entry;

		throw out_of_range("LSQ tag not found");
	}
};
