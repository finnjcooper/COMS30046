#include "simulator.hpp"

void Simulator::load(const vector<uint8_t> &bytes, const string &name) {
	Program program = Loader::elf_bytes(bytes, name);
	cpu->load(program);
}

void Simulator::configure(const Config &config) {
	cpu = make_unique<CPU>(config);
}

void Simulator::reset() {
	cpu->reset();
}

void Simulator::step() {
	cpu->step();
}

void Simulator::run() {
	while (!cpu->halted()) cpu->step();
}

Snapshot Simulator::snapshot() {
	return cpu->snapshot();
}
