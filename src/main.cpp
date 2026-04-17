#include "loader.hpp"
#include "tui.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("RISC-V Simulator");
	program.add_argument("--elf", "-e").help("path to the ELF binary to load and simulate").required();
	program.add_argument("--headless", "-h").help("run without the TUI").default_value(false).implicit_value(true);
	program.add_argument("--config", "-c").help("path to the configuration file").default_value("./config.json");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception &e) {
		cerr << e.what() << endl;
		cerr << program.help().str();
		return 1;
	}

	string elfPath = program.get<string>("--elf");
	string asmPath = elfPath.substr(0, elfPath.size() - 4) + ".asm";

	auto prog = Loader::ELF(elfPath);
	auto disasm = Loader::ASM(asmPath);
	auto config = Loader::config(program.get<string>("--config"));

	CPU cpu(prog, config);
	TUI tui(cpu, disasm);

	if (program.get<bool>("--headless"))
		while (cpu.running()) cpu.step();
	else tui.run();

	auto instr_count = cpu.get_instruction_count();
	auto cycle_count = cpu.get_cycle_count();
	auto ipc = static_cast<float>(instr_count) / max(cycle_count, 1);
	auto branch_preds = cpu.get_branch_preds();
	auto branch_mispreds = cpu.get_branch_mispreds();
	auto mispred_rate = branch_preds ? 100.0 * branch_mispreds / (branch_mispreds + branch_preds) : 0.0;

	cout << "Instructions executed: " << instr_count << endl;
	cout << "Cycles taken: " << cycle_count << endl;
	cout << "IPC: " << fixed << setprecision(3) << ipc << endl;
	cout << "Branch predictions: " << branch_preds << endl;
	cout << "Branch mispredictions: " << branch_mispreds << endl;
	cout << "Branch misprediction rate: " << fixed << setprecision(2) << mispred_rate << "%" << endl;

	return 0;
};
