#include "loader.hpp"
#include <fstream>
#include <regex>
#include <elfio/elfio.hpp>

Program Loader::ELF(const string &filename) {
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
		if (seg->get_type() != ELFIO::PT_LOAD) continue;

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

map<uint32_t, string> Loader::ASM(const string &filename) {
	map<uint32_t, string> disasm;
	ifstream file(filename);
	string line;
	
	// regex to match lines like: "  a4:	00200793          	li	a5,2"
	regex instrPattern("^\\s*([0-9a-f]+):\\s+([0-9a-f]+)\\s+(.+)$");
	
	while (getline(file, line)) {
		smatch match;
		if (regex_match(line, match, instrPattern)) {
			uint32_t addr = stoul(match[1].str(), nullptr, 16);
			string instr = match[3].str();
			disasm[addr] = instr;
		}
	}
	
	return disasm;
}
