#include "loader.hpp"
#include <fstream>
#include <elfio/elfio.hpp>

Program Loader::ELF(const string &filename) {
	ELFIO::elfio elf;
	if (!elf.load(filename)) {
		cerr << "Could not open ELF file: " << filename << endl;
		return { vector<uint8_t>(), 0, 0 };
	}

	uint32_t mem_size = 0;	
	for (const auto& seg : elf.segments) {
		if (seg->get_type() == ELFIO::PT_LOAD) {
			uint32_t end = seg->get_virtual_address() + seg->get_memory_size();
			mem_size = std::max(mem_size, end);
		}
	}

	uint32_t exit_point = 0;
	for (const auto &sec : elf.sections) {
		if (sec->get_name() == ".text") {
			exit_point = static_cast<uint32_t>(sec->get_address() + sec->get_size());
			break;
		}
	}

	if (exit_point == 0) exit_point = mem_size;

	auto memory = vector<uint8_t>(mem_size, 0);
	for (const auto& seg : elf.segments) {
		if (seg->get_type() != ELFIO::PT_LOAD) continue;

		uint32_t vaddr = seg->get_virtual_address();
		uint32_t file_sz = seg->get_file_size();
		uint32_t mem_sz  = seg->get_memory_size();

		const char* data = seg->get_data();

		for (uint32_t i = 0; i < mem_sz; i++) {
			memory[vaddr + i] = i >= file_sz ? 0 : data[i];
		}
	}

	return { memory, static_cast<uint32_t>(elf.get_entry()), exit_point };
}

map<uint32_t, string> Loader::ASM(const string &filename) {

	map<uint32_t, string> disasm;
	ifstream file(filename);
	string line;

	while (getline(file, line)) {
		auto colon = line.find(':');
		if (colon == string::npos) continue;

		string addr_str = line.substr(0, colon);
		addr_str.erase(0, addr_str.find_first_not_of(" \t"));

		if (addr_str.empty()) continue;
		if (!isxdigit(addr_str[0])) continue;

		uint32_t addr = stoul(addr_str, nullptr, 16);

		auto bytes_end = line.find_first_not_of(" \t", colon + 1);
		if (bytes_end == string::npos) continue;

		auto mnemonic_start = line.find_first_of(" \t", bytes_end);
		if (mnemonic_start == string::npos) continue;

		auto mnemonic_end = line.find_first_not_of(" \t", mnemonic_start);
		if (mnemonic_end == string::npos) continue;

		string instr = line.substr(mnemonic_end);
		disasm[addr] = instr;
	}
	
	return disasm;
}
