#include "loader.hpp"

namespace {

Program empty_program() {
	return { "", vector<uint8_t>(), 0, 0 };
}

Program load_elf_stream(istream &stream, const string &source_name) {
	ELFIO::elfio elf;
	if (!elf.load(stream)) {
		cerr << "Could not open ELF data: " << source_name << endl;
		return empty_program();
	}

	uint32_t mem_size = 0;
	for (const auto& seg : elf.segments) {
		if (seg->get_type() == ELFIO::PT_LOAD) {
			uint32_t vaddr = seg->get_virtual_address();
			uint32_t seg_size = seg->get_memory_size();
			if (vaddr > MEM_SIZE || seg_size > MEM_SIZE - vaddr) {
				cerr << "ELF segment does not fit in simulator memory: " << source_name << endl;
				return empty_program();
			}
			mem_size = max(mem_size, vaddr + seg_size);
		}
	}

	uint32_t code_end = 0;
	for (const auto& section : elf.sections) {
		if ((section->get_flags() & ELFIO::SHF_EXECINSTR) == 0) continue;
		uint32_t text_addr = section->get_address();
		uint32_t text_size = section->get_size();
		if (text_addr > MEM_SIZE || text_size > MEM_SIZE - text_addr) {
			cerr << "ELF executable section does not fit in simulator memory: " << source_name << endl;
			return empty_program();
		}
		code_end = max(code_end, text_addr + text_size);
	}
	if (!code_end) code_end = mem_size;

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

	return { source_name, memory, static_cast<uint32_t>(elf.get_entry()), code_end };
}

map<uint32_t, string> load_asm_stream(istream &stream) {
	map<uint32_t, string> disasm;
	string line;

	while (getline(stream, line)) {
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

Config load_config_text(const string &json_text, const string &source_name) {
	Config config;

	try {
		auto json = nlohmann::json::parse(json_text);
		config.name = json.value("name", config.name);
		config.pipe_width = json.value("pipe_width", config.pipe_width);
		config.rs_size = json.value("rs_size", config.rs_size);
		config.lsq_size = json.value("lsq_size", config.lsq_size);
		config.alu_count = json.value("alu_count", config.alu_count);
		config.mul_count = json.value("mul_count", config.mul_count);
		config.ctrl_count = json.value("ctrl_count", config.ctrl_count);
		config.fpu_count = json.value("fpu_count", config.fpu_count);
		config.vec_count = json.value("vec_count", config.vec_count);
		config.lsu_count = json.value("lsu_count", config.lsu_count);
		config.vector_bits = json.value("vector_bits", config.vector_bits);
		config.branch_pred = json.value("branch_pred", config.branch_pred);
	} catch (const nlohmann::json::exception &e) {
		cerr << "Could not parse config data: " << source_name << " (" << e.what() << "). Using default config." << endl;
	}

	return config;
}

}

Program Loader::elf(const string &filename) {
	ifstream file(filename, ios::binary);
	if (!file.is_open()) {
		cerr << "Could not open ELF file: " << filename << endl;
		return empty_program();
	}

	return load_elf_stream(file, filename);
}

Program Loader::elf_bytes(const vector<uint8_t> &bytes, const string &name) {
	istringstream stream(string(bytes.begin(), bytes.end()), ios::in | ios::binary);
	return load_elf_stream(stream, name);
}

Config Loader::config(const string &filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Could not open config file: " << filename << ". Using default config." << endl;
		return Config{};
	}

	ostringstream buffer;
	buffer << file.rdbuf();
	return config_text(buffer.str());
}

Config Loader::config_text(const string &json_text) {
	return load_config_text(json_text, "in-memory config JSON");
}
