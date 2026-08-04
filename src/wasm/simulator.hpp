#pragma once

#include <emscripten/val.h>
#include "cpu.hpp"
#include "loader.hpp"

class Simulator {
public:
	Simulator();
	bool loadElf(emscripten::val bytes);
	// get program name?
	void loadConfig(const string &data);
	bool build();
	bool step();
	bool run(uint32_t max_steps);
	bool reset();

	void setStepCallback(emscripten::val callback);

private:
	void attachStepCallback();

	Program program;
	Config config;
	unique_ptr<CPU> cpu;
	emscripten::val step_callback = emscripten::val::undefined();
};
