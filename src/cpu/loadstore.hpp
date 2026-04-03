#pragma once
#include <cstdint>
#include <optional>

class LoadStoreUnit {
public:
	optional<uint32_t> step() {
		if (!busy_) return nullopt;
		if (--cycles_remaining != 0) return nullopt;

		busy_ = false;
		return current_tag;
	}

	void start(uint32_t tag) {
		current_tag = tag;
		cycles_remaining = cycles;
		busy_ = true;
	}

	bool busy() const { return busy_; }

	void flush(uint32_t tag) {
		if (busy_ && current_tag > tag) busy_ = false;
	}

private:
	static constexpr size_t cycles = 1UL;

	size_t cycles_remaining = 0;
	uint32_t current_tag = -1U;
	bool busy_ = false;
};
