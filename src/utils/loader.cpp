#include "loader.hpp"
#include <fstream>
#include <elfio/elfio.hpp>
#include <nlohmann/json.hpp>

Program Loader::ELF(const string &filename) {
	ELFIO::elfio elf;
	if (!elf.load(filename)) {
		cerr << "Could not open ELF file: " << filename << endl;
		return { vector<uint8_t>(), 0 };
	}

	uint32_t mem_size = 0;
	for (const auto& seg : elf.segments) {
		if (seg->get_type() == ELFIO::PT_LOAD) {
			uint32_t seg_end = seg->get_virtual_address() + seg->get_memory_size();
			mem_size = max(mem_size, seg_end);
		}
	}

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

	return { memory, static_cast<uint32_t>(elf.get_entry()) };
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

Config Loader::config(const string &filename) {
	Config config;
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Could not open config file: " << filename << ". Using default config." << endl;
		return config;
	}

	try {
		auto json = nlohmann::json::parse(file);
		config.pipe_width = json.value("pipe-width", config.pipe_width);
		config.rs_size = json.value("rs-size", config.rs_size);
		config.lsq_size = json.value("lsq-size", config.lsq_size);
		config.alu_count = json.value("alu-count", config.alu_count);
		config.mul_count = json.value("mul-count", config.mul_count);
		config.ctrl_count = json.value("ctrl-count", config.ctrl_count);
		config.vec_count = json.value("vec-count", config.vec_count);
		config.lsu_count = json.value("lsu-count", config.lsu_count);
		config.branch_pred = json.value("branch", config.branch_pred);
	} catch (const nlohmann::json::exception &e) {
		cerr << "Could not parse config file: " << filename << " (" << e.what() << "). Using default config." << endl;
	}

	return config;
}
