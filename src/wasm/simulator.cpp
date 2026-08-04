#include "simulator.hpp"

Simulator::Simulator() {}

bool Simulator::loadElf(emscripten::val bytes) {
	const size_t size = bytes["length"].as<size_t>();
    vector<uint8_t> buffer(size);
    emscripten::val memory = emscripten::val(emscripten::typed_memory_view(size, buffer.data()));
    memory.call<void>("set", bytes);

	program = Loader::elf_bytes(buffer);
	return !program.instrs.empty();
}

void Simulator::loadConfig(const string &data) {
	config = Loader::config_text(data);
}

bool Simulator::build() {
	if (program.instrs.empty()) return false;

	cpu = make_unique<CPU>(program, config);
	attachStepCallback();
	return true;
}

bool Simulator::step() {
	if (!cpu || !cpu->running()) return false;

	cpu->step();
	return cpu->running();
}

bool Simulator::run(uint32_t max_steps) {
	if (!cpu) return false;

	uint32_t count = 0;
	while (cpu->running() && count < max_steps) {
		cpu->step();
		count++;
	}

	return cpu->running();
}

bool Simulator::reset() {
	if (!cpu) return false;

	cpu = make_unique<CPU>(program, config);
	attachStepCallback();
	return true;
}

void Simulator::setStepCallback(emscripten::val callback) {
	step_callback = callback;
	attachStepCallback();
}

void Simulator::attachStepCallback() {
	if (!cpu || step_callback.isUndefined() || step_callback.isNull()) return;

	cpu->set_step_callback([this](bool jumped, bool stalled, uint32_t pc, const Stats &stats, const CommitLog &log, const string &readout) {
		auto snapshot = emscripten::val::object();
		snapshot.set("jumped", jumped);
		snapshot.set("stalled", stalled);
		snapshot.set("pc", pc);
		snapshot.set("stats", stats);
		// snapshot.set("log", log);
		snapshot.set("readout", readout);
		step_callback(snapshot);
	});
}
