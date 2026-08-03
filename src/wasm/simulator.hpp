#pragma once

#include <emscripten/val.h>
#include "cpu.hpp"
#include "loader.hpp"

class Simulator {
public:
	Simulator();
	bool loadElf(emscripten::val bytes);
	void loadConfig(const string &data);
	bool build();
	void reset();
	bool step();
	bool run(uint32_t max_steps);
	uint32_t pc() const;
	uint32_t registerValue(int index) const;
	string readout() const;

private:
	Program program;
	Config config;
	unique_ptr<CPU> cpu;
};
