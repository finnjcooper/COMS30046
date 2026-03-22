#pragma once
#include <sstream>
#include <functional>
#include "regfile.hpp"
#include "memory.hpp"
#include "alu.hpp"
#include "mul.hpp"
#include "branch.hpp"
#include "loadstore.hpp"
#include "instruction.hpp"
#include "decode.hpp"
#include "pipeline.hpp"
#include "tomasulo.hpp"
#include "trace.hpp"
#include "loader.hpp"

using namespace std;

class CPU {
public:
	~CPU() = default;
	CPU(Program prog, bool pipelined = true, bool forwarding = true);

	static constexpr size_t MEM_SIZE = 64 * 1024; // 64 KB
	static constexpr size_t LSU_COUNT = 2, BRU_COUNT = 2, MUL_COUNT = 1, ALU_COUNT = 2;

	void step();

	bool running() { return !halted; }
	RegisterFile getRegisters() const { return regs; }
	Memory getMemory() const { return mem; }
	Pipeline getPipeline() const { return pipe; }
	uint32_t getPC() const { return pc; }
	int getInstructionCount() const { return instruction_count; }
	int getCycleCount() const { return cycle_count; }
	CommitLog getCommitLog() const { return log; }

	void setStepCallback(function<void(bool, const CommitLog &, const string &)> callback) { onStepCallback = callback; }

	string readout();

private:
	uint32_t pc = 0;
	uint32_t end = 0;
	uint32_t seq = 0;
	uint32_t next_commit = 0;
	bool jumped = false;
	bool halted = false;
	bool pipelined = true;

	int instruction_count = 0;
	int cycle_count = 0;
	CommitLog log;
	Memory mem;
	RegisterFile regs;
	ROB rob;
	vector<LoadStoreUnit> lsus = vector<LoadStoreUnit>(LSU_COUNT, LoadStoreUnit(mem, log));
	vector<BranchUnit> brus = vector<BranchUnit>(BRU_COUNT, BranchUnit(end));
	vector<MulUnit> muls = vector<MulUnit>(MUL_COUNT);
	vector<ALU> alus = vector<ALU>(ALU_COUNT);
	Pipeline pipe;

	ostringstream out;
	function<void(bool, const CommitLog &, const string &)> onStepCallback;

	void stepSequential();

	void fetch();
	void decode();
	void issue();
	void execute();
	void writeback();
	void commit();
};
