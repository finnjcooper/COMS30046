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

	static constexpr size_t MEM_SIZE = 64 * 1024ULL; // 64KB
	static constexpr uint8_t WORD_BYTES = 4ULL, NUM_REGISTERS = 32ULL, NUM_FLOAT_REGISTERS = 32ULL;

	void step();

	bool running() const { return !halted; }
	const RegisterFile& get_registers() const { return regs; }
	const Memory& get_memory() const { return mem; }
	uint32_t get_pc() const { return pc; }
	int get_instruction_count() const { return instruction_count; }
	int get_cycle_count() const { return cycle_count; }
	int get_branch_preds() const { return branch_preds; }
	int get_branch_mispreds() const { return branch_mispreds; }
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
	int branch_preds = 0;
	int branch_mispreds = 0;

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

	void read_operand(uint8_t rs, uint32_t &V, uint32_t &Q, RegisterFile &regs, RegisterAliasTable &rat);
	ExecPath& get_path(Op op);
	RegisterFile& get_regfile(Op op);
	RegisterAliasTable& get_rat(Op op);
	void flush(uint32_t tag);

	void fetch();
	void decode();
	void dispatch();
	void issue();
	void execute();
	void writeback();
	void commit();
};
