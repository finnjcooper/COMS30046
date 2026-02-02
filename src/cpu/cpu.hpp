#pragma once
#include <sstream>
#include <functional>
#include "regfile.hpp"
#include "memory.hpp"
#include "alu.hpp"
#include "branch.hpp"
#include "loadstore.hpp"
#include "instruction.hpp"
#include "decoder.hpp"
#include "pipeline.hpp"
#include "trace.hpp"
#include "loader.hpp"

using namespace std;

class CPU {
public:
	~CPU() = default;
	CPU(Program prog, bool isPipelined = true, bool isForwarding = true);

	static constexpr size_t MEM_SIZE = 64 * 1024; // 64 KB
	static constexpr uint8_t XLEN = 32;
	static constexpr uint8_t WORD_BYTES = XLEN / 8;

	void step();

	bool running() { return !halted; }
	RegisterFile getRegisters() const { return regs; }
	Memory getMemory() const { return mem; }
	Pipeline getPipeline() const { return pipe; }
	uint32_t getPC() const { return pc; }
	int getInstructionCount() const { return instructionCount; }
	int getCycleCount() const { return cycleCount; }
	CommitLog getCommitLog() const { return log; }

	void setStepCallback(function<void(const PipelineControl &, const CommitLog &, const string &)> callback) { onStepCallback = callback; }

	string readout();

private:
	RegisterFile regs;
	LoadStoreUnit lsu;
	BranchUnit bru;
	ALU alu;
	Memory mem;
	Pipeline pipe;

	ostringstream out;
	function<void(const PipelineControl &, const CommitLog &, const string &)> onStepCallback;
	CommitLog log;

	uint32_t pc = 0;
	bool halted = false;
	bool pipelined = true;

	int instructionCount = 0;
	int cycleCount = 0;

	void stepSequential();

	bool fetch();
	bool decode();
	void execute();
	void memory();
	bool writeback();
};
