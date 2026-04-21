#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <elfio/elfio.hpp>
#include <nlohmann/json.hpp>
#include "isa.hpp"

using namespace std;

struct Program {
	vector<uint8_t> instrs;
	uint32_t entry_point;
	uint32_t end_point;
};

struct Config {
	string name = "Baseline";
	size_t pipe_width = 2, rs_size = 8, lsq_size = 8;
	size_t alu_count = 2, mul_count = 1, ctrl_count = 1, fpu_count = 1, vec_count = 1, lsu_count = 2;
	uint32_t vector_bits = 128;
	string branch_pred = "two_bit";

	friend ostream& operator<<(ostream& os, const Config& config) {
		os << "Config:\n";
		os << "  Pipe width: " << config.pipe_width << "\n";
		os << "  Reservation station size: " << config.rs_size << "\n";
		os << "  Load/store queue size: " << config.lsq_size << "\n";
		os << "  ALU count: " << config.alu_count << "\n";
		os << "  MULU count: " << config.mul_count << "\n";
		os << "  CTRLU count: " << config.ctrl_count << "\n";
		os << "  FPU count: " << config.fpu_count << "\n";
		os << "  VECU count: " << config.vec_count << "\n";
		os << "  LSU count: " << config.lsu_count << "\n";
		os << "  Vector bits: " << config.vector_bits << "\n";
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
