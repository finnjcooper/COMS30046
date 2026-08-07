#pragma once
#include <sstream>
#include <functional>
#include <deque>
#include "helpers.hpp"
#include "regfile.hpp"
#include "memory.hpp"
#include "decode.hpp"
#include "branch.hpp"
#include "rob.hpp"
#include "rat.hpp"
#include "alu.hpp"
#include "mul.hpp"
#include "ctrl.hpp"
#include "fpu.hpp"
#include "vec.hpp"
#include "lsu.hpp"
#include "exec_path.hpp"
#include "loader.hpp"

struct FetchEntry {
	uint32_t pc = 0;
	uint32_t instr = 0;
};

struct Stats {
	int instruction_count = 0;
	int cycle_count = 0;
	int branch_count = 0;
	int mispred_count = 0;
};

struct Snapshot {
	uint32_t pc = 0;
	Stats stats = {};
};

class CPU {
public:
	~CPU() = default;
	CPU(const Config &config);

	void load(const Program &program);
	void step();
	bool running() const { return !halted; }

	string readout() {
		string s = out.str();
		out.str("");
		out.clear();
		return s;
	};

	Snapshot snapshot() {
		return Snapshot {
			pc,
			stats,
		};
	}

private:
	uint32_t pc = 0, end = 0;
	bool jumped = false;
	bool stalled = false;
	bool halted = false;
	bool fetch_stopped = false;

	Stats stats;

	size_t width = 0;

	CommitLog log, flog, vlog;
	Memory mem;
	IntegerRegisterFile regs;
	FloatRegisterFile fregs;
	VectorRegisterFile vregs;
	ReOrderBuffer rob;
	RegisterAliasTable rat, frat, vrat;
	VectorState vec_state;
	ExecPath alus, muls, ctrls, fpus, vecs;
	LoadStorePath lsus;
	array<ExecPath*, 6ULL> exec_paths;
	unique_ptr<BranchPredictor> branch_pred;

	deque<FetchEntry> fetch_q;
	deque<DecodeEntry> decode_q;

	ostringstream out;

	void read_operand(uint8_t rs, RegType type, Value &V, uint32_t &Q);
	ExecPath& get_path(Op op);
	RegisterFile& regfile(RegType type);
	RegisterAliasTable& alias_table(RegType type);
	void flush(uint32_t tag);
	void check_halt();

	void fetch();
	void decode();
	void dispatch();
	void issue();
	void execute();
	void writeback();
	void commit();
};
