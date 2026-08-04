#include "simulator.hpp"
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(simulator) {
	value_object<Stats>("Stats")
		.field("instructionCount", &Stats::instruction_count)
		.field("cycleCount", &Stats::cycle_count)
		.field("branchCount", &Stats::branch_count)
		.field("mispredCount", &Stats::mispred_count);


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
			"step",
			&Simulator::step
		)

		.function(
			"run",
			&Simulator::run
		)

		.function(
			"reset",
			&Simulator::reset
		)

		.function(
			"setStepCallback",
			&Simulator::setStepCallback
		);
}
