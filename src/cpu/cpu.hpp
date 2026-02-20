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
#include "trace.hpp"
#include "loader.hpp"

using namespace std;

class CPU {
public:
	~CPU() = default;
	CPU(Program prog, bool pipelined = true, bool forwarding = true);

	static constexpr size_t MEM_SIZE = 64 * 1024; // 64 KB
	static constexpr uint8_t XLEN = 32;
	static constexpr uint8_t WORD_BYTES = XLEN / 8;

	void step();

	bool running() { return !halted; }
	RegisterFile getRegisters() const { return regs; }
	Memory getMemory() const { return mem; }
	Pipeline getPipeline() const { return pipe; }
	uint32_t getPC() const { return pc; }
	int getInstructionCount() const { return instruction_count; }
	int getCycleCount() const { return cycle_count; }
	CommitLog getCommitLog() const { return log; }

	void setStepCallback(function<void(const PipelineControl &, const CommitLog &, const string &)> callback) { onStepCallback = callback; }

	string readout();

private:
	RegisterFile regs;
	LoadStoreUnit lsu;
	BranchUnit bru;
	MulUnit mul;
	ALU alu;
	Memory mem;
	Pipeline pipe;

	ostringstream out;
	function<void(const PipelineControl &, const CommitLog &, const string &)> onStepCallback;
	CommitLog log;

	uint32_t pc = 0;
	uint32_t end = 0;
	bool halted = false;
	bool pipelined = true;

	int instruction_count = 0;
	int cycle_count = 0;

	void stepSequential();

	void fetch();
	void decode();
	void execute();
	void memory();
	void writeback();
};
