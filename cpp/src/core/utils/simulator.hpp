#pragma once

#include "cpu.hpp"
#include "loader.hpp"

class Simulator {
public:
	Simulator();
	bool load(const vector<uint8_t> &bytes);
	// get program name?
	bool configure(const string &text);
	bool step();
	bool running() const;
	string readout();
	Snapshot snapshot() const;

private:
	unique_ptr<CPU> cpu;
};
