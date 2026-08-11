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

	Value V = 0;
	uint32_t Va = 0;
	uint32_t Q = -1U;
	uint32_t Qa = -1U;
	uint8_t vl = 0;
	uint8_t sew = 0;

	bool issued = false;
	bool done = false;
};

class LoadStoreQueue {
public:
	LoadStoreQueue(size_t size) : max_size(size) {}

	void clear() {
		entries.clear();
	}

	vector<LSQEntry> snapshot() const {
		return vector<LSQEntry>(entries.begin(), entries.end());
	}

	bool can_allocate() const {
		return entries.size() < max_size;
	}

	void allocate(const RSEntry &entry) {
		LSQEntry lsq_entry;
		lsq_entry.op = entry.op;
		lsq_entry.tag = entry.tag;
		lsq_entry.V = is_store(entry.op) ? entry.Vk : Value::scalar(0);
		lsq_entry.Q = is_store(entry.op) ? entry.Qk : -1U;
		lsq_entry.Qa = entry.tag;
		lsq_entry.vl = is_vload(entry.op) || is_vstore(entry.op) ? entry.vl : 0;
		lsq_entry.sew = is_vload(entry.op) || is_vstore(entry.op) ? op_sew(entry.op) : 0;
		entries.push_back(lsq_entry);
	}

	const deque<LSQEntry>& get_entries() const { return entries; }

	optional<LSQEntry> issue() {
		for (auto &entry : entries) {
			if (!can_issue(entry)) continue;
			entry.issued = true;
			return entry;
		}

		return nullopt;
	}

	void update(const ExecEntry &entry) {
		auto &lsq_entry = get(entry.tag);
		lsq_entry.Va = entry.value.as_scalar();
		lsq_entry.Qa = -1U;
		if (is_vload(lsq_entry.op) || is_vstore(lsq_entry.op))
			lsq_entry.vl = entry.vl;
	}

	ExecEntry complete(uint32_t tag, const Memory &mem) {
		auto &entry = get(tag);
		size_t load_idx = index_of(tag);

		if (is_vload(entry.op)) {
			entry.V = vload_forward(entry, load_idx, mem);
		} else if (is_load(entry.op)) {
			uint32_t raw = load_forward(entry, load_idx, mem);
			entry.V = format_load(entry.op, raw);
		}

		entry.issued = false;
		entry.done = true;
		return ExecEntry {entry.op, entry.V, 0, false, entry.tag, entry.vl, entry.sew};
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

	void wake(const ExecEntry &exec) {
		for (auto &entry : entries) {
			if (entry.done) continue;
			if (is_store(entry.op) && entry.Q == exec.tag) {
				entry.Q = -1U;
				entry.V = exec.value;
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

	uint32_t access_size(const LSQEntry &entry) const {
		if (is_vload(entry.op) || is_vstore(entry.op))
			return entry.vl * (entry.sew / 8);

		switch (entry.op) {
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

	static Value format_load(Op op, uint32_t raw) {
		switch (op) {
			case LB: return Value::scalar(sign_extend(raw & 0xFF, 8));
			case LH: return Value::scalar(sign_extend(raw & 0xFFFF, 16));
			case LW:
			case FLW: return Value::scalar(raw);
			case LBU: return Value::scalar(raw & 0xFF);
			case LHU: return Value::scalar(raw & 0xFFFF);
			default: return Value::scalar(0);
		}
	}

	static uint32_t load(const LSQEntry &entry, const Memory &mem) {
		switch (entry.op) {
			case LB: case LBU: return mem.loadb(entry.Va);
			case LH: case LHU: return mem.loadh(entry.Va);
			case LW: case FLW: return mem.loadw(entry.Va);
			default: return 0;
		}
	}

		static void store(const LSQEntry &entry, Memory &mem, CommitLog &log) {
		switch (entry.op) {
			case SB: {
				uint8_t old_val = mem.loadb(entry.Va), new_val = entry.V.as_scalar() & 0xFF;
				mem.storeb(entry.Va, new_val);
				log.record_mem_write(entry.Va, old_val, new_val, 1);
				break;
			}
			case SH: {
				uint16_t old_val = mem.loadh(entry.Va), new_val = entry.V.as_scalar() & 0xFFFF;
				mem.storeh(entry.Va, new_val);
				log.record_mem_write(entry.Va, old_val, new_val, 2);
				break;
			}
			case SW:
			case FSW: {
				uint32_t old_val = mem.loadw(entry.Va);
				mem.storew(entry.Va, entry.V.as_scalar());
				log.record_mem_write(entry.Va, old_val, entry.V.as_scalar(), 4);
				break;
			}
			case VSE8_V:
			case VSE16_V:
			case VSE32_V: {
				uint8_t bytes = entry.sew / 8;
				for (uint8_t lane = 0; lane < entry.vl; lane++) {
					uint32_t addr = entry.Va + lane * bytes;
					uint32_t value = entry.V.lane(lane);

					switch (entry.op) {
						case VSE8_V: {
							uint8_t old_val = mem.loadb(addr), new_val = value & 0xFF;
							mem.storeb(addr, new_val);
							log.record_mem_write(addr, old_val, new_val, 1);
							break;
						}
						case VSE16_V: {
							uint16_t old_val = mem.loadh(addr), new_val = value & 0xFFFF;
							mem.storeh(addr, new_val);
							log.record_mem_write(addr, old_val, new_val, 2);
							break;
						}
						case VSE32_V: {
							uint32_t old_val = mem.loadw(addr);
							mem.storew(addr, value);
							log.record_mem_write(addr, old_val, value, 4);
							break;
						}
						default:
							break;
					}
				}
				break;
			}
			default:
				break;
		}
	}

	bool covers_byte(const LSQEntry &store_entry, uint64_t byte_addr) const {
		uint64_t start = store_entry.Va;
		uint64_t end = start + access_size(store_entry);
		return byte_addr >= start && byte_addr < end;
	}

	uint8_t byte_from(const LSQEntry &store_entry, uint64_t byte_addr) const {
		uint64_t offset = byte_addr - store_entry.Va;
		if (is_vstore(store_entry.op)) {
			uint8_t bytes = store_entry.sew / 8;
			if (bytes == 0) return 0;
			size_t lane = static_cast<size_t>(offset / bytes);
			uint8_t lane_byte = static_cast<uint8_t>(offset % bytes);
			uint32_t lane_value = store_entry.V.lane(lane);
			return static_cast<uint8_t>((lane_value >> (lane_byte * 8)) & 0xFF);
		}

		uint8_t store_byte = static_cast<uint8_t>(offset);
		uint32_t word = store_entry.V.as_scalar();
		return static_cast<uint8_t>((word >> (store_byte * 8)) & 0xFF);
	}

	optional<uint8_t> byte_forward(size_t load_idx, uint64_t byte_addr) const {
		optional<uint8_t> result;
		for (size_t i = 0; i < load_idx; i++) {
			const auto &store_entry = entries[i];
			if (!is_store(store_entry.op)) continue;
			if (!covers_byte(store_entry, byte_addr)) continue;
			result = byte_from(store_entry, byte_addr);
		}
		return result;
	}

	Value vload_forward(const LSQEntry &load_entry, size_t load_idx, const Memory &mem) const {
		Value value = Value::vector_zero();
		uint8_t bytes = load_entry.sew / 8;
		if (bytes == 0) return value;

		for (uint8_t lane = 0; lane < load_entry.vl; lane++) {
			uint32_t lane_value = 0;
			uint32_t lane_offset = static_cast<uint32_t>(lane) * bytes;

			for (uint8_t byte = 0; byte < bytes; byte++) {
				uint64_t addr = static_cast<uint64_t>(load_entry.Va) + lane_offset + byte;
				uint8_t lane_byte = mem.loadb(static_cast<uint32_t>(addr));
				auto forwarded = byte_forward(load_idx, addr);
				if (forwarded) lane_byte = *forwarded;
				lane_value |= static_cast<uint32_t>(lane_byte) << (byte * 8);
			}

			value.set_lane(lane, lane_value);
		}

		return value;
	}

	uint32_t load_forward(const LSQEntry &load_entry, size_t load_idx, const Memory &mem) const {
		uint32_t raw = load(load_entry, mem);
		uint8_t load_size = access_size(load_entry);

		for (uint8_t load_byte = 0; load_byte < load_size; load_byte++) {
			uint64_t byte_addr = static_cast<uint64_t>(load_entry.Va) + load_byte;
			auto forwarded = byte_forward(load_idx, byte_addr);
			if (!forwarded) continue;
			uint32_t shift = load_byte * 8;
			raw = (raw & ~(0xFFU << shift)) | (static_cast<uint32_t>(*forwarded) << shift);
		}

		return raw;
	}

	bool can_issue(const LSQEntry &entry) const {
		if (entry.issued || entry.done || entry.Qa != -1U) return false;
		if (is_store(entry.op) && entry.Q != -1U) return false;
		if (is_load(entry.op) && !can_issue_load(entry)) return false;
		return true;
	}

	bool can_issue_load(const LSQEntry &load_entry) const {
		size_t load_idx = index_of(load_entry.tag);

		for (size_t i = 0; i < load_idx; i++) {
			const auto &entry = entries[i];
			if (!is_store(entry.op)) continue;
			if (entry.Qa != -1U) return false;
			if (overlaps(entry, load_entry) && entry.Q != -1U) return false;
		}

		return true;
	}

	bool overlaps(const LSQEntry &a, const LSQEntry &b) const {
		uint64_t a_start = a.Va;
		uint64_t b_start = b.Va;
		uint64_t a_end = a_start + access_size(a);
		uint64_t b_end = b_start + access_size(b);

		return a_start < b_end && b_start < a_end;
	}

	LSQEntry& get(uint32_t tag) {
		for (auto &entry : entries)
			if (entry.tag == tag) return entry;

		throw out_of_range("LSQ tag not found");
	}

	size_t index_of(uint32_t tag) const {
		for (size_t i = 0; i < entries.size(); i++)
			if (entries[i].tag == tag) return i;

		throw out_of_range("LSQ tag not found");
	}
};
