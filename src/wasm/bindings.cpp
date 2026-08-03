#include "simulator.hpp"
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(simulator) {
	class_<Simulator>("Simulator")
		.constructor<>()

		.function(
			"loadElf",
			&Simulator::loadElf
		)

		.function(
			"loadConfig",
			&Simulator::loadConfig
		)

		.function(
			"build",
			&Simulator::build
		)

		.function(
			"reset",
			&Simulator::reset
		)

		.function(
			"step",
			&Simulator::step
		)

		.function(
			"run",
			&Simulator::run
		)

		.function(
			"pc",
			&Simulator::pc
		)

		.function(
			"registerValue",
			&Simulator::registerValue
		)

		.function(
			"readout",
			&Simulator::readout
		);
}
