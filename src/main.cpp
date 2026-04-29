#include "tui.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("RISC-V Simulator");
	program.add_argument("elf").help("path to the ELF binary to load and simulate");
	program.add_argument("--config", "-c").help("path to the configuration file").default_value("");
	program.add_argument("--results", "-r").help("path to the results CSV file").default_value("experiments/results.csv");
	program.add_argument("--headless", "-h").help("run without the TUI").default_value(false).implicit_value(true);

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception &e) {
		cerr << e.what() << endl;
		cerr << program.help().str();
		return 1;
	}

	string elfPath = program.get<string>("elf");
	string asmPath = elfPath.substr(0, elfPath.size() - 4) + ".asm";

	auto prog = Loader::ELF(elfPath);
	if (prog.instrs.empty()) return 1;
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
	auto branch_count = cpu.get_branch_count();
	auto mispred_count = cpu.get_mispred_count();
	auto mispred_rate = branch_count ? 100.0 * mispred_count / branch_count : 0.0;

	auto filename = program.get<string>("--results");
	ifstream existing_results(filename, ios::ate);
	const bool write_header = !existing_results || existing_results.tellg() == 0;
	ofstream results(filename, ios::app);
	if (!results) {
		cerr << "Unable to open " << filename << endl;
		return 1;
	}

	if (write_header) results << "Config,Benchmark,Instructions,Cycles,IPC,Branches,Prediction Rate (%)\n";

	auto bench_start = elfPath.find_last_of("/\\");
	string bench = elfPath.substr(bench_start == string::npos ? 0 : bench_start + 1);
	auto ext_start = bench.find_last_of('.');
	if (ext_start != string::npos) bench = bench.substr(0, ext_start);

	results << config.name << "," << bench << "," << instr_count << "," << cycle_count << "," << fixed << setprecision(3) << ipc << "," << branch_count << "," << 100.0 - mispred_rate << "\n";
	results.close();

	cout << "Instructions executed: " << instr_count << endl;
	cout << "Cycles taken: " << cycle_count << endl;
	cout << "IPC: " << fixed << setprecision(3) << ipc << endl;
	cout << "Branches: " << branch_count << endl;
	cout << "Mispredictions: " << mispred_count << " (" << fixed << setprecision(2) << mispred_rate << "%)" << endl;

	return 0;
};
