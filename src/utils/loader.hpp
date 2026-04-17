#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <elfio/elfio.hpp>
#include <nlohmann/json.hpp>

using namespace std;

struct Program {
	vector<uint8_t> instrs;
	uint32_t entry_point;
};

struct Config {
	size_t pipe_width = 1, rs_size = 8, lsq_size = 8;
	size_t alu_count = 1, mul_count = 1, ctrl_count = 1, vec_count = 1, lsu_count = 1;
	string branch_pred = "static_taken";

	friend ostream& operator<<(ostream& os, const Config& config) {
		os << "Config:\n";
		os << "  Pipe width: " << config.pipe_width << "\n";
		os << "  Reservation station size: " << config.rs_size << "\n";
		os << "  Load/store queue size: " << config.lsq_size << "\n";
		os << "  ALU count: " << config.alu_count << "\n";
		os << "  Mul/Div unit count: " << config.mul_count << "\n";
		os << "  Control unit count: " << config.ctrl_count << "\n";
		os << "  Vector unit count: " << config.vec_count << "\n";
		os << "  LSU count: " << config.lsu_count;
		os << "  Branch prediction: " << config.branch_pred << "\n";
		return os;
	}
};

class Loader {
public:
	static Program ELF(const string &filename);
	static map<uint32_t, string> ASM(const string &filename);
	static Config config(const string &filename);
};
