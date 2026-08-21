#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <cctype>
#include <sstream>
#include <iostream>
#include <fstream>
#include <elfio/elfio.hpp>
#include <nlohmann/json.hpp>
#include "isa.hpp"

using namespace std;

struct Program {
	string name;
	vector<uint8_t> instrs;
	uint32_t entry_point;
	uint32_t end_point;
};

struct Config {
	string name = "Default";
	size_t pipe_width = 2, rs_size = 8, lsq_size = 8;
	size_t alu_count = 2, mul_count = 2, ctrl_count = 2, fpu_count = 2, vec_count = 2, lsu_count = 2;
	uint32_t vector_bits = 128;
	PredictorType branch_pred = PredictorType::TWO_BIT;

	friend ostream& operator<<(ostream& os, const Config& config) {
		os << "Config:\n";
		os << "  Pipe width: " << config.pipe_width << "\n";
		os << "  Reservation station size: " << config.rs_size << "\n";
		os << "  Load/store queue size: " << config.lsq_size << "\n";
		os << "  ALU count: " << config.alu_count << "\n";
		os << "  MUL count: " << config.mul_count << "\n";
		os << "  CTRL count: " << config.ctrl_count << "\n";
		os << "  FPU count: " << config.fpu_count << "\n";
		os << "  VEC count: " << config.vec_count << "\n";
		os << "  LSU count: " << config.lsu_count << "\n";
		os << "  Vector bits: " << config.vector_bits << "\n";
		os << "  Branch prediction: " << config.branch_pred << "\n";
		return os;
	}
};

class Loader {
public:
	static Program elf(const string &filename);
	static Program elf_bytes(const vector<uint8_t> &bytes, const string &name);
	static Config config(const string &filename);
	static Config config_text(const string &json_text);
};
