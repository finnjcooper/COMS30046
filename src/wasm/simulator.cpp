#include "simulator.hpp"

Simulator::Simulator() {}

bool Simulator::loadElf(emscripten::val bytes) {
	const size_t size = bytes["length"].as<size_t>();

    vector<uint8_t> buffer(size);

    emscripten::val memory =
        emscripten::val(emscripten::typed_memory_view(size, buffer.data()));

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
	return true;
}


void Simulator::reset() {
	build();
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


uint32_t Simulator::pc() const {
	return cpu ? cpu->get_pc() : 0;
}


uint32_t Simulator::registerValue(int index) const {
	if (!cpu) return 0;

	return cpu->get_registers()
		.read(index)
		.as_scalar();
}


string Simulator::readout() const {
	return cpu ? cpu->readout() : "";
}
