#pragma once
#include <sstream>
#include <functional>
#include <deque>
#include "instruction.hpp"
#include "pipeline.hpp"
#include "regfile.hpp"
#include "memory.hpp"
#include "decode.hpp"
#include "branch.hpp"
#include "rob.hpp"
#include "alu.hpp"
#include "mul.hpp"
#include "ctrl.hpp"
#include "lsu.hpp"
#include "exec_path.hpp"
#include "loader.hpp"

using namespace std;

class CPU {
public:
	~CPU() = default;
	CPU(Program prog);

	static constexpr size_t MEM_SIZE = 64 * 1024ULL; // 64 KB
	static constexpr uint8_t XLEN = 32U, WORD_BYTES = XLEN / 8, NUM_REGISTERS = 32U;
	static constexpr size_t CORE_WIDTH = 2ULL, RS_SIZE = 4ULL, LSQ_SIZE = 8ULL;
	static constexpr size_t LSU_COUNT = 1ULL, CTRL_COUNT = 1ULL, MUL_COUNT = 1ULL, ALU_COUNT = 2ULL;

	void step();

	bool running() const { return !halted; }
	const RegisterFile& get_registers() const { return regs; }
	const Memory& get_memory() const { return mem; }
	uint32_t get_pc() const { return pc; }
	int get_instruction_count() const { return instruction_count; }
	int get_cycle_count() const { return cycle_count; }
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

	CommitLog log;
	Memory mem;
	RegisterFile regs;
	ReOrderBuffer rob;
	RegisterAliasTable rat;
	ExecPath alus, muls, ctrls;
	LoadStorePath lsus;
	array<ExecPath*, 4> exec_paths;
	BranchPredictor* bp;

	deque<FetchEntry> fetch_q;
	deque<DecodeEntry> decode_q;

	ostringstream out;
	function<void(bool, bool, const CommitLog &, const string &)> on_step_callback;

	void read_operand(uint8_t rs, uint32_t &V, uint32_t &Q);
	ExecPath& get_path(Op op);
	void flush(uint32_t tag);

	void fetch();
	void decode();
	void dispatch();
	void issue();
	void execute();
	void writeback();
	void commit();
};
