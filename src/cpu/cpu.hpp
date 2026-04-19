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

class CPU {
public:
	~CPU() = default;
	CPU(const Program &prog, const Config &config);

	void step();

	bool running() const { return !halted; }
	const RegisterFile& get_registers() const { return regs; }
	const Memory& get_memory() const { return mem; }
	uint32_t get_pc() const { return pc; }
	int get_instruction_count() const { return instruction_count; }
	int get_cycle_count() const { return cycle_count; }
	int get_branch_count() const { return branch_count; }
	int get_mispred_count() const { return mispred_count; }
	const CommitLog& get_commit_log() const { return log; }

	void set_step_callback(function<void(bool, bool, const CommitLog &, const string &)> callback) { on_step_callback = callback; }

	string readout();

private:
	uint32_t pc = 0, end = 0;
	bool jumped = false;
	bool stalled = false;
	bool halted = false;

	int instruction_count = 0;
	int cycle_count = 0;
	int branch_count = 0;
	int mispred_count = 0;

	size_t width = 0;

	CommitLog log;
	CommitLog flog;
	Memory mem;
	IntegerRegisterFile regs;
	FloatRegisterFile fregs;
	ReOrderBuffer rob;
	RegisterAliasTable rat, frat;
	ExecPath alus, muls, ctrls, fpus, vecs;
	LoadStorePath lsus;
	array<ExecPath*, 6ULL> exec_paths;
	unique_ptr<BranchPredictor> branch_pred;

	deque<FetchEntry> fetch_q;
	deque<DecodeEntry> decode_q;

	ostringstream out;
	function<void(bool, bool, const CommitLog &, const string &)> on_step_callback;

	void read_operand(uint8_t rs, RegType type, uint32_t &V, uint32_t &Q);
	ExecPath& get_path(Op op);
	RegisterFile& regfile(RegType type);
	RegisterAliasTable& alias_table(RegType type);
	void flush(uint32_t tag);

	void fetch();
	void decode();
	void dispatch();
	void issue();
	void execute();
	void writeback();
	void commit();
};
