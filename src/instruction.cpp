#include "instruction.h"

Program Loader::fromBinary(const string& filename) {
	ifstream file(filename, ios::binary);
	auto bytes = vector<uint8_t>();
	char byte;
	while (file.read(&byte, 1)) {
		bytes.push_back(byte);
	};

	return { bytes, 0 };
}

Program Loader::fromElf(const string& filename) {
	ELFIO::elfio elf;
	if (!elf.load(filename)) {
		cerr << "Could not open ELF file: " << filename << endl;
		return { vector<uint8_t>(), 0 };
	}

	uint32_t mem_size = 0;
	for (const auto& seg : elf.segments) {
		if (seg->get_type() == ELFIO::PT_LOAD) {
			uint32_t end = seg->get_virtual_address() + seg->get_memory_size();
			mem_size = std::max(mem_size, end);
		}
	}

	auto memory = vector<uint8_t>(mem_size, 0);
	for (const auto& seg : elf.segments) {
		if (seg->get_type() != ELFIO::PT_LOAD) {
			continue;
		}

		uint32_t vaddr = seg->get_virtual_address();
		uint32_t filesz = seg->get_file_size();
		uint32_t memsz  = seg->get_memory_size();

		const char* data = seg->get_data();

		for (uint32_t i = 0; i < memsz; i++) {
			memory[vaddr + i] = i >= filesz ? 0 : data[i];
		}
	}

	return { memory, static_cast<uint32_t>(elf.get_entry()) };
}