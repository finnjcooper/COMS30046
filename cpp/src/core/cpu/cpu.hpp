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

struct FetchState {
	FetchState() = default;
	FetchState(const FetchEntry &e) : pc(e.pc), instr(e.instr) {}
	uint32_t pc = 0;
	uint32_t instr = 0;
};

struct Stats {
	int instruction_count = 0;
	int cycle_count = 0;
	int branch_count = 0;
	int mispred_count = 0;
};

typedef uint32_t Register;

struct VectorRegister {
	VectorRegister() = default;
	VectorRegister(array<uint32_t, MAX_VECTOR_LANES> lanes) { this->lanes.assign(lanes.begin(), lanes.end()); }
	vector<uint32_t> lanes;
};

struct RegisterState {
	vector<Register> rat;
	vector<Register> frat;
	vector<Register> vrat;

	vector<Register> regs;
	vector<Register> fregs;
	vector<VectorRegister> vregs;

	VectorState vec_state;
};

struct PipelineState {
	vector<FetchState> fetch_q;
	vector<DecodeState> decode_q;

	vector<ROBState> rob;

	ExecPathState alus;
	ExecPathState muls;
	ExecPathState ctrls;
	ExecPathState fpus;
	ExecPathState vecs;
	LoadStoreState lsus;
};

struct Snapshot {
	string program_name;
	string config_name;

	bool halted = false;
	bool stalled = false;
	bool jumped = false;
	uint32_t pc = 0;

	Stats stats;

	RegisterState registers;
	PipelineState pipeline;
};

class CPU {
public:
	~CPU() = default;
	CPU(const Config &config);

	void reset();
	void load(const Program &program);
	void step();
	bool halted() const { return halted_; }
	string get_program_name() const { return program_name; }
	string get_config_name() const { return config_name; }
	Snapshot snapshot() const;
	string readout();

private:
	uint32_t pc = 0, entry = 0, end = 0;
	bool jumped = false;
	bool stalled = false;
	bool halted_ = false;
	bool fetch_stopped = false;

	Stats stats;

	string program_name;
	string config_name;

	size_t width = 0;

	CommitLog log, flog, vlog;
	Memory mem;
	IntegerRegisterFile regs;
	FloatRegisterFile fregs;
	VectorRegisterFile vregs;
	ReOrderBuffer rob;
	RegisterAliasTable rat, frat, vrat;
	VectorConfig vec_config;
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
