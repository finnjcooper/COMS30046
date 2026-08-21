#pragma once

#include "cpu.hpp"
#include "loader.hpp"

class Simulator {
public:
	Simulator() = default;
	void load(const vector<uint8_t> &bytes, const string &name);
	void configure(const Config &config);
	void reset();
	void step();
	void run();
	Snapshot snapshot();

private:
	unique_ptr<CPU> cpu;
};
