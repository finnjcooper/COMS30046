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

class CPU {
public:
	~CPU() = default;
	CPU(Program prog, bool isPipelined = true, bool isForwarding = true) :
		mem(Memory(prog.instrs, MEM_SIZE)), regs(log), lsu(mem, log),
		pipe(Pipeline(isPipelined, isForwarding)), pc(prog.entryPoint) {
		regs.write(2, MEM_SIZE - WORD_BYTES);
	}

	static constexpr size_t MEM_SIZE = 64 * 1024; // 64 KB
	static constexpr uint8_t XLEN = 32;
	static constexpr uint8_t WORD_BYTES = XLEN / 8;

	void step() {
		if (!pipe.isPipelined()) { stepUnpipelined(); return; }

		log.clear();
		cycles++;
		
		PipelineControl ctrl = pipe.getControl();
		if (pipe.memwb.valid) instructions++;

		if (pipe.writeback(regs)) halted = true;
		pipe.memory(lsu);

		// control hazard
		if (ctrl.jumped) {
			pc = ctrl.target;
			pipe.flush();
			out << "Control hazard: flushing pipeline, jumping to 0x" << hex << setw(8) << setfill('0') << pc << dec << "\n";
			return;
		}

		pipe.execute(alu, bru);

		// data hazard
		if (!ctrl.stall) {
			pipe.decode(regs);
			if (pipe.fetch(pc, mem)) pc += WORD_BYTES;
		} else {
			out << "Data hazard: stalling pipeline\n";
		}

		if (onStepCallback) onStepCallback(log, readout());
	}

	bool running() { return !halted; }
	RegisterFile getRegisters() const { return regs; }
	Memory getMemory() const { return mem; }
	Pipeline getPipeline() const { return pipe; }
	uint32_t getPC() const { return pc; }
	int getNumInstructions() const { return instructions; }
	int getNumCycles() const { return cycles; }
	CommitLog getCommitLog() const { return log; }

	void setStepCallback(function<void(const CommitLog &, const string &)> callback) { onStepCallback = callback; }

	string readout() {
		string s = out.str();
		out.str("");
		out.clear();
		return s;
	}

private:
	RegisterFile regs;
	LoadStoreUnit lsu;
	BranchUnit bru;
	ALU alu;
	Memory mem;
	Pipeline pipe;

	ostringstream out;
	function<void(const CommitLog &, const string &)> onStepCallback;
	CommitLog log;

	uint32_t pc = 0;
	bool halted = false;

	int instructions = 0;
	int cycles = 0;

	void stepUnpipelined() {
		log.clear();

		pipe.fetch(pc, mem);
		pc += WORD_BYTES;

		pipe.decode(regs);
		pipe.execute(alu, bru);

		PipelineControl ctrl = pipe.getControl();
		if (ctrl.jumped) pc = ctrl.target;

		pipe.memory(lsu);
		if (pipe.writeback(regs)) halted = true;

		instructions++;
		cycles += 5;

		if (onStepCallback) onStepCallback(log, readout());
	}
};
