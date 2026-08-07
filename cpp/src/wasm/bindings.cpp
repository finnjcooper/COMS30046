#include "simulator.hpp"
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

namespace {

vector<uint8_t> to_vector(val bytes) {
	const size_t size = bytes["length"].as<size_t>();
	vector<uint8_t> buffer(size);
	val memory = val(typed_memory_view(size, buffer.data()));
	memory.call<void>("set", bytes);
	return buffer;
}

}

EMSCRIPTEN_BINDINGS(simulator) {
	value_object<Stats>("Stats")
		.field("instructionCount", &Stats::instruction_count)
		.field("cycleCount", &Stats::cycle_count)
		.field("branchCount", &Stats::branch_count)
		.field("mispredCount", &Stats::mispred_count);
	
	value_object<Snapshot>("Snapshot")
		.field("pc", &Snapshot::pc)
		.field("stats", &Snapshot::stats);


	class_<Simulator>("Simulator")
		.constructor<>()

		.function(
			"load",
			optional_override([](Simulator& sim, val bytes) {
				return sim.load(to_vector(bytes));
			})
		)

		.function(
			"configure",
			&Simulator::configure
		)

		.function(
			"step",
			&Simulator::step
		)

		.function(
			"running",
			&Simulator::running
		)

		.function(
			"readout",
			&Simulator::readout
		)

		.function(
			"snapshot",
			&Simulator::snapshot
		);
}
