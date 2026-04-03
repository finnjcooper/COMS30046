#pragma once
#include <sstream>
#include <functional>
#include <deque>
#include "instruction.hpp"
#include "pipeline.hpp"
#include "regfile.hpp"
#include "memory.hpp"
#include "decode.hpp"
#include "rob.hpp"
#include "lsq.hpp"
#include "alu.hpp"
#include "mul.hpp"
#include "branch.hpp"
#include "loadstore.hpp"
#include "exec_path.hpp"
#include "loader.hpp"

using namespace std;

class CPU {
public:
	~CPU() = default;
	CPU(Program prog);

	static constexpr size_t MEM_SIZE = 64 * 1024ULL; // 64 KB
	static constexpr uint8_t XLEN = 32U, WORD_BYTES = XLEN / 8, NUM_REGISTERS = 32U;
	static constexpr size_t PIPELINE_WIDTH = 2ULL, RS_SIZE = 4ULL;
	static constexpr size_t LSU_COUNT = 2ULL, CTRL_COUNT = 1ULL, MUL_COUNT = 1ULL, ALU_COUNT = 2ULL;

	void step();

	bool running() const { return !halted; }
	const RegisterFile& getRegisters() const { return regs; }
	const Memory& getMemory() const { return mem; }
	uint32_t getPC() const { return pc; }
	int getInstructionCount() const { return instruction_count; }
	int getCycleCount() const { return cycle_count; }
	const CommitLog& getCommitLog() const { return log; }

	void setStepCallback(function<void(bool, const CommitLog &, const string &)> callback) { onStepCallback = callback; }

	string readout();

private:
	uint32_t pc = 0;
	bool jumped = false;
	bool halted = false;

	int instruction_count = 0;
	int cycle_count = 0;

	CommitLog log;
	Memory mem;
	RegisterFile regs;
	ReOrderBuffer rob;
	LoadStoreQueue lsq;
	RegisterAliasTable rat;
	ExecPath alus, muls, ctrls;
	LoadStoreExecPath lsus;
	array<ExecPath*, 4> exec_paths;

	deque<FetchEntry> fetch_q;
	deque<DecodeEntry> decode_q;

	ostringstream out;
	function<void(bool, const CommitLog &, const string &)> onStepCallback;

	void readOperand(uint8_t rs, uint32_t &V, uint32_t &Q);
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
