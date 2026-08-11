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
	register_vector<FetchEntry>("FetchQueue");
	register_vector<DecodeEntry>("DecodeQueue");
	register_vector<Value>("Regs");
	register_vector<uint32_t>("RAT");
	register_vector<ROBEntry>("ROB");
	register_vector<LSQEntry>("LSQ");
	register_vector<RSEntry>("ReservationStations");
	register_vector<ExecSnapshot>("ExecUnits");
	register_vector<LSUSnapshot>("LSUs");

	class_<Value>("Value")
		.constructor<uint32_t>()
		.property("vector_value", &Value::vector_value)
		.function("is_vector", &Value::is_vector)
		.function("as_scalar", &Value::as_scalar)
		.function("lane", &Value::lane)
		.function("set_lane", &Value::set_lane);

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

	value_object<Instruction>("Instruction")
		.field("op", &Instruction::op)
		.field("rd", &Instruction::rd)
		.field("rs1", &Instruction::rs1)
		.field("rs2", &Instruction::rs2)
		.field("imm", &Instruction::imm)
		.field("rs3", &Instruction::rs3)
		.field("rm", &Instruction::rm)
		.field("sew", &Instruction::sew);

	value_object<VectorStateSnapshot>("VectorState")
		.field("tag", &VectorStateSnapshot::tag)
		.field("vector_bits", &VectorStateSnapshot::vector_bits)
		.field("vsew_bits", &VectorStateSnapshot::vsew_bits)
		.field("vl", &VectorStateSnapshot::vl);

	value_object<Stats>("Stats")
		.field("instructionCount", &Stats::instruction_count)
		.field("cycleCount", &Stats::cycle_count)
		.field("branchCount", &Stats::branch_count)
		.field("mispredCount", &Stats::mispred_count);

	value_object<ROBEntry>("ROBEntry")
		.field("ready", &ROBEntry::ready)
		.field("jumped", &ROBEntry::jumped)
		.field("ctrl_handled", &ROBEntry::ctrl_handled)
		.field("pred_taken", &ROBEntry::pred_taken)
		.field("pred_target", &ROBEntry::pred_target)
		.field("op", &ROBEntry::op)
		.field("rd", &ROBEntry::rd)
		.field("value", &ROBEntry::value)
		.field("target", &ROBEntry::target)
		.field("pc", &ROBEntry::pc)
		.field("vl", &ROBEntry::vl)
		.field("sew", &ROBEntry::sew)
		.field("tag", &ROBEntry::tag);

	value_object<RSEntry>("RSEntry")
		.field("busy", &RSEntry::busy)
		.field("op", &RSEntry::op)
		.field("Vj", &RSEntry::Vj)
		.field("Vk", &RSEntry::Vk)
		.field("Vl", &RSEntry::Vl)
		.field("Qj", &RSEntry::Qj)
		.field("Qk", &RSEntry::Qk)
		.field("Ql", &RSEntry::Ql)
		.field("Qv", &RSEntry::Qv)
		.field("pc", &RSEntry::pc)
		.field("imm", &RSEntry::imm)
		.field("rm", &RSEntry::rm)
		.field("vl", &RSEntry::vl)
		.field("sew", &RSEntry::sew)
		.field("tag", &RSEntry::tag);

	value_object<LSQEntry>("LSQEntry")
		.field("op", &LSQEntry::op)
		.field("tag", &LSQEntry::tag)
		.field("V", &LSQEntry::V)
		.field("Va", &LSQEntry::Va)
		.field("Q", &LSQEntry::Q)
		.field("Qa", &LSQEntry::Qa)
		.field("vl", &LSQEntry::vl)
		.field("sew", &LSQEntry::sew)
		.field("issued", &LSQEntry::issued)
		.field("done", &LSQEntry::done);

	value_object<ExecEntry>("ExecEntry")
		.field("op", &ExecEntry::op)
		.field("value", &ExecEntry::value)
		.field("target", &ExecEntry::target)
		.field("jumped", &ExecEntry::jumped)
		.field("tag", &ExecEntry::tag)
		.field("vl", &ExecEntry::vl)
		.field("sew", &ExecEntry::sew);

	value_object<ExecSnapshot>("ExecSnapshot")
		.field("busy", &ExecSnapshot::busy)
		.field("cycles_remaining", &ExecSnapshot::cycles_remaining)
		.field("current", &ExecSnapshot::current);

	value_object<LSUSnapshot>("LSUSnapshot")
		.field("busy", &LSUSnapshot::busy)
		.field("cycles_remaining", &LSUSnapshot::cycles_remaining)
		.field("current_tag", &LSUSnapshot::current_tag);

	value_object<ExecPathSnapshot>("ExecPathSnapshot")
		.field("units", &ExecPathSnapshot::units)
		.field("stations", &ExecPathSnapshot::stations);

	value_object<LoadStorePathSnapshot>("LoadStorePathSnapshot")
		.field("agus", &LoadStorePathSnapshot::agus)
		.field("lsus", &LoadStorePathSnapshot::lsus)
		.field("lsq", &LoadStorePathSnapshot::lsq);

	value_object<RATSnapshot>("RATSnapshot")
		.field("table", &RATSnapshot::table);

	value_object<ROBSnapshot>("ROBSnapshot")
		.field("next_tag", &ROBSnapshot::next_tag)
		.field("entries", &ROBSnapshot::entries);

	value_object<RegisterFileSnapshot>("RegisterFileSnapshot")
		.field("regs", &RegisterFileSnapshot::regs);
	
	value_object<FetchEntry>("FetchEntry")
		.field("pc", &FetchEntry::pc)
		.field("instr", &FetchEntry::instr);
	
	value_object<DecodeEntry>("DecodeEntry")
		.field("pc", &DecodeEntry::pc)
		.field("instr", &DecodeEntry::instr)
		.field("predTaken", &DecodeEntry::pred_taken)
		.field("predTarget", &DecodeEntry::pred_target);

	value_object<Snapshot>("Snapshot")
		.field("program", &Snapshot::program_name)
		.field("config", &Snapshot::config_name)
		.field("halted", &Snapshot::halted)
		.field("stalled", &Snapshot::stalled)
		.field("jumped", &Snapshot::jumped)
		.field("msg", &Snapshot::msg)
		.field("stats", &Snapshot::stats)
		.field("pc", &Snapshot::pc)
		.field("vecState", &Snapshot::vec_state)
		.field("fetchQ", &Snapshot::fetch_q)
		.field("decodeQ", &Snapshot::decode_q)
		.field("rob", &Snapshot::rob)
		.field("rat", &Snapshot::rat)
		.field("frat", &Snapshot::frat)
		.field("vrat", &Snapshot::vrat)
		.field("alus", &Snapshot::alus)
		.field("muls", &Snapshot::muls)
		.field("ctrls", &Snapshot::ctrls)
		.field("fpus", &Snapshot::fpus)
		.field("vecs", &Snapshot::vecs)
		.field("lsus", &Snapshot::lsus)
		.field("regs", &Snapshot::regs)
		.field("fregs", &Snapshot::fregs)
		.field("vregs", &Snapshot::vregs);


	class_<Simulator>("Simulator")
		.constructor<>()
		.function("load", optional_override([](Simulator &sim, val bytes, string name) { return sim.load(to_vector(bytes), name); }))
		.function("configure", &Simulator::configure)
		.function("reset", &Simulator::reset)
		.function("step", &Simulator::step)
		.function("run", &Simulator::run)
		.function("snapshot", &Simulator::snapshot);
}
