#include "simulator.hpp"

Simulator::Simulator() {}

bool Simulator::load(const vector<uint8_t> &bytes) {
	Program program = Loader::elf_bytes(bytes);
	cpu->load(program);
	return !program.instrs.empty();
}

bool Simulator::configure(const string &text) {
	Config config = Loader::config_text(text);
	cpu = make_unique<CPU>(config);
	return config.name != "Baseline";
}

bool Simulator::step() {
	if (!cpu || !cpu->running()) return false;
	cpu->step();
	return true;
}

bool Simulator::running() const {
	if (!cpu) return false;
	return cpu->running();
}

string Simulator::readout() {
	if (!cpu) return "";
	return cpu->readout();
}

Snapshot Simulator::snapshot() const {
	if (!cpu) return Snapshot{};
	return cpu->snapshot();
}
