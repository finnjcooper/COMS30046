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
	register_vector<FetchEntry>("FetchQueue");
	register_vector<DecodeEntry>("DecodeQueue");
	register_vector<Value>("Regs");
	register_vector<uint32_t>("RAT");
	register_vector<ROBEntry>("ROB");
	register_vector<LSQEntry>("LSQ");
	register_vector<RSEntry>("ReservationStations");
	register_vector<ExecSnapshot>("ExecUnits");
	register_vector<LSUSnapshot>("LSUs");

	value_object<Stats>("Stats")
		.field("instructionCount", &Stats::instruction_count)
		.field("cycleCount", &Stats::cycle_count)
		.field("branchCount", &Stats::branch_count)
		.field("mispredCount", &Stats::mispred_count);
	
	value_object<FetchEntry>("FetchEntry")
		.field("pc", &FetchEntry::pc)
		.field("instr", &FetchEntry::instr);
	
	value_object<DecodeEntry>("DecodeEntry")
		.field("pc", &DecodeEntry::pc)
		// .field("instr", &DecodeEntry::instr)
		.field("predTaken", &DecodeEntry::pred_taken)
		.field("predTarget", &DecodeEntry::pred_target);

	value_object<Snapshot>("Snapshot")
		.field("program", &Snapshot::program_name)
		.field("config", &Snapshot::config_name)
		.field("halted", &Snapshot::halted)
		.field("stalled", &Snapshot::stalled)
		.field("jumped", &Snapshot::jumped)
		.field("msg", &Snapshot::msg)
		.field("stats", &Snapshot::stats)
		.field("pc", &Snapshot::pc)
		.field("vecState", &Snapshot::vec_state)
		.field("fetchQ", &Snapshot::fetch_q)
		.field("decodeQ", &Snapshot::decode_q)
		.field("rob", &Snapshot::rob)
		.field("rat", &Snapshot::rat)
		.field("frat", &Snapshot::frat)
		.field("vrat", &Snapshot::vrat)
		.field("alus", &Snapshot::alus)
		.field("muls", &Snapshot::muls)
		.field("ctrls", &Snapshot::ctrls)
		.field("fpus", &Snapshot::fpus)
		.field("vecs", &Snapshot::vecs)
		.field("lsus", &Snapshot::lsus)
		.field("regs", &Snapshot::regs)
		.field("fregs", &Snapshot::fregs)
		.field("vregs", &Snapshot::vregs);


	class_<Simulator>("Simulator")
		.constructor<>()
		.function("load", optional_override([](Simulator &sim, val bytes, string name) { return sim.load(to_vector(bytes), name); }))
		.function("configure", &Simulator::configure)
		.function("reset", &Simulator::reset)
		.function("step", &Simulator::step)
		.function("run", &Simulator::run)
		.function("snapshot", &Simulator::snapshot);
}
