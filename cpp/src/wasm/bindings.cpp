#include "simulator.hpp"
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

namespace {

vector<uint8_t> to_vector(val bytes) {
	const size_t size = bytes["length"].as<size_t>();
	vector<uint8_t> buffer(size);
	val memory = val(typed_memory_view(size, buffer.data()));
	memory.call<void>("set", bytes);
	return buffer;
}

}

EMSCRIPTEN_BINDINGS(simulator) {
	register_vector<FetchState>("FetchQueue");
	register_vector<DecodeState>("DecodeQueue");
	register_vector<Register>("Registers");
	register_vector<VectorRegister>("VectorRegisters");
	register_vector<ROBState>("ROB");
	register_vector<LSQState>("LSQ");
	register_vector<RSState>("ReservationStations");
	register_vector<ExecState>("ExecUnits");

	enum_<Op>("Op")
		.value("INVALID", Op::INVALID)
		// RV32I
		.value("ADD", Op::ADD)
		.value("SUB", Op::SUB)
		.value("SLL", Op::SLL)
		.value("SLT", Op::SLT)
		.value("SLTU", Op::SLTU)
		.value("XOR", Op::XOR)
		.value("SRL", Op::SRL)
		.value("SRA", Op::SRA)
		.value("OR", Op::OR)
		.value("AND", Op::AND)
		.value("ADDI", Op::ADDI)
		.value("SLTI", Op::SLTI)
		.value("SLTIU", Op::SLTIU)
		.value("XORI", Op::XORI)
		.value("ORI", Op::ORI)
		.value("ANDI", Op::ANDI)
		.value("SLLI", Op::SLLI)
		.value("SRLI", Op::SRLI)
		.value("SRAI", Op::SRAI)
		.value("LB", Op::LB)
		.value("LH", Op::LH)
		.value("LW", Op::LW)
		.value("LBU", Op::LBU)
		.value("LHU", Op::LHU)
		.value("SB", Op::SB)
		.value("SH", Op::SH)
		.value("SW", Op::SW)
		.value("BEQ", Op::BEQ)
		.value("BNE", Op::BNE)
		.value("BLT", Op::BLT)
		.value("BGE", Op::BGE)
		.value("BLTU", Op::BLTU)
		.value("BGEU", Op::BGEU)
		.value("JAL", Op::JAL)
		.value("JALR", Op::JALR)
		.value("LUI", Op::LUI)
		.value("AUIPC", Op::AUIPC)
		// RV32M
		.value("MUL", Op::MUL)
		.value("MULH", Op::MULH)
		.value("MULHSU", Op::MULHSU)
		.value("MULHU", Op::MULHU)
		.value("DIV", Op::DIV)
		.value("DIVU", Op::DIVU)
		.value("REM", Op::REM)
		.value("REMU", Op::REMU)
		// RV32F
		.value("FADD_S", Op::FADD_S)
		.value("FSUB_S", Op::FSUB_S)
		.value("FMUL_S", Op::FMUL_S)
		.value("FDIV_S", Op::FDIV_S)
		.value("FSQRT_S", Op::FSQRT_S)
		.value("FMADD_S", Op::FMADD_S)
		.value("FMSUB_S", Op::FMSUB_S)
		.value("FNMSUB_S", Op::FNMSUB_S)
		.value("FNMADD_S", Op::FNMADD_S)
		.value("FSGNJ_S", Op::FSGNJ_S)
		.value("FSGNJN_S", Op::FSGNJN_S)
		.value("FSGNJX_S", Op::FSGNJX_S)
		.value("FMIN_S", Op::FMIN_S)
		.value("FMAX_S", Op::FMAX_S)
		.value("FEQ_S", Op::FEQ_S)
		.value("FLT_S", Op::FLT_S)
		.value("FLE_S", Op::FLE_S)
		.value("FLW", Op::FLW)
		.value("FSW", Op::FSW)
		.value("FMV_X_W", Op::FMV_X_W)
		.value("FMV_W_X", Op::FMV_W_X)
		.value("FCVT_W_S", Op::FCVT_W_S)
		.value("FCVT_WU_S", Op::FCVT_WU_S)
		.value("FCVT_S_W", Op::FCVT_S_W)
		.value("FCVT_S_WU", Op::FCVT_S_WU)
		.value("FCLASS_S", Op::FCLASS_S)
		// RV32V
		.value("VSETVLI", Op::VSETVLI)
		.value("VSETIVLI", Op::VSETIVLI)
		.value("VLE8_V", Op::VLE8_V)
		.value("VLE16_V", Op::VLE16_V)
		.value("VLE32_V", Op::VLE32_V)
		.value("VSE8_V", Op::VSE8_V)
		.value("VSE16_V", Op::VSE16_V)
		.value("VSE32_V", Op::VSE32_V)
		.value("VMUL_VV", Op::VMUL_VV)
		.value("VMACC_VV", Op::VMACC_VV)
		.value("VMACC_VX", Op::VMACC_VX)
		.value("VMV_V_I", Op::VMV_V_I)
		.value("VMV_V_X", Op::VMV_V_X)
		.value("VMV_S_X", Op::VMV_S_X)
		.value("VMV_X_S", Op::VMV_X_S)
		.value("VMV1R_V", Op::VMV1R_V)
		.value("VREDSUM_VS", Op::VREDSUM_VS)
		.value("VFMV_V_F", Op::VFMV_V_F)
		.value("VFMACC_VV", Op::VFMACC_VV)
		.value("VFMACC_VF", Op::VFMACC_VF);

	value_object<Stats>("Stats")
		.field("instructionCount", &Stats::instruction_count)
		.field("cycleCount", &Stats::cycle_count)
		.field("branchCount", &Stats::branch_count)
		.field("mispredCount", &Stats::mispred_count);

	value_object<VectorState>("VectorState")
		.field("tag", &VectorState::tag)
		.field("vector_bits", &VectorState::vector_bits)
		.field("vsew_bits", &VectorState::vsew_bits)
		.field("vl", &VectorState::vl);

	value_object<VectorRegister>("VectorRegister")
		.field("lanes", &VectorRegister::lanes);

	value_object<RSState>("RSState")
		.field("op", &RSState::op)
		.field("Vj", &RSState::Vj)
		.field("Vk", &RSState::Vk)
		.field("Vl", &RSState::Vl)
		.field("Qj", &RSState::Qj)
		.field("Qk", &RSState::Qk)
		.field("Ql", &RSState::Ql)
		.field("Qv", &RSState::Qv)
		.field("pc", &RSState::pc)
		.field("tag", &RSState::tag);

	value_object<LSQState>("LSQState")
		.field("op", &LSQState::op)
		.field("tag", &LSQState::tag)
		.field("V", &LSQState::V)
		.field("Va", &LSQState::Va)
		.field("Q", &LSQState::Q)
		.field("Qa", &LSQState::Qa);

	value_object<ExecState>("ExecState")
		.field("busy", &ExecState::busy)
		.field("op", &ExecState::op)
		.field("tag", &ExecState::tag)
		.field("cycles_remaining", &ExecState::cycles_remaining);

	value_object<ExecPathState>("ExecPathState")
		.field("units", &ExecPathState::units)
		.field("stations", &ExecPathState::stations);

	value_object<LoadStoreState>("LoadStoreState")
		.field("agus", &LoadStoreState::agus)
		.field("lsus", &LoadStoreState::lsus)
		.field("stations", &LoadStoreState::stations)
		.field("lsq", &LoadStoreState::lsq);

	value_object<ROBState>("ROBState")
		.field("jumped", &ROBState::jumped)
		.field("target", &ROBState::target)
		.field("pred_taken", &ROBState::pred_taken)
		.field("pred_target", &ROBState::pred_target)
		.field("op", &ROBState::op)
		.field("pc", &ROBState::pc)
		.field("tag", &ROBState::tag);

	value_object<FetchState>("FetchState")
		.field("pc", &FetchState::pc)
		.field("instr", &FetchState::instr);

	value_object<DecodeState>("DecodeState")
		.field("pc", &DecodeState::pc)
		.field("op", &DecodeState::op)
		.field("predTaken", &DecodeState::pred_taken)
		.field("predTarget", &DecodeState::pred_target);

	value_object<RegisterState>("RegisterState")
		.field("vec_state", &RegisterState::vec_state)
		.field("rat", &RegisterState::rat)
		.field("frat", &RegisterState::frat)
		.field("vrat", &RegisterState::vrat)
		.field("regs", &RegisterState::regs)
		.field("fregs", &RegisterState::fregs)
		.field("vregs", &RegisterState::vregs);

	value_object<PipelineState>("PipelineState")
		.field("fetch_q", &PipelineState::fetch_q)
		.field("decode_q", &PipelineState::decode_q)
		.field("rob", &PipelineState::rob)
		.field("alus", &PipelineState::alus)
		.field("muls", &PipelineState::muls)
		.field("ctrls", &PipelineState::ctrls)
		.field("fpus", &PipelineState::fpus)
		.field("vecs", &PipelineState::vecs)
		.field("lsus", &PipelineState::lsus);

	value_object<Snapshot>("Snapshot")
		.field("program", &Snapshot::program_name)
		.field("config", &Snapshot::config_name)
		.field("halted", &Snapshot::halted)
		.field("stalled", &Snapshot::stalled)
		.field("jumped", &Snapshot::jumped)
		.field("pc", &Snapshot::pc)
		.field("stats", &Snapshot::stats)
		.field("registers", &Snapshot::registers)
		.field("pipeline", &Snapshot::pipeline);


	class_<Simulator>("Simulator")
		.constructor<>()
		.function("load", optional_override([](Simulator &sim, val bytes, string name) { return sim.load(to_vector(bytes), name); }))
		.function("configure", &Simulator::configure)
		.function("reset", &Simulator::reset)
		.function("step", &Simulator::step)
		.function("run", &Simulator::run)
		.function("snapshot", &Simulator::snapshot);
}
