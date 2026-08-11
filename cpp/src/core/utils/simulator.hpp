#pragma once

#include "cpu.hpp"
#include "loader.hpp"

class Simulator {
public:
	Simulator();
	void load(const vector<uint8_t> &bytes, const string &name);
	// get program name?
	void configure(const string &text);
	void reset();
	void step();
	void run();
	Snapshot snapshot();

private:
	unique_ptr<CPU> cpu;
};
