#include "simulator.hpp"

Simulator::Simulator() {
	cpu = make_unique<CPU>(Config());
}

void Simulator::load(const vector<uint8_t> &bytes, const string &name) {
	Program program = Loader::elf_bytes(bytes, name);
	cpu->load(program);
}

void Simulator::configure(const string &text) {
	Config config = Loader::config_text(text);
	cpu = make_unique<CPU>(config);
}

void Simulator::reset() {
	cpu->reset();
}

void Simulator::step() {
	cpu->step();
}

void Simulator::run() {
	while (!cpu->get_halted()) cpu->step();
}

Snapshot Simulator::snapshot() {
	return cpu->snapshot();
}
