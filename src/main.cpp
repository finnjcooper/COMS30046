#include "loader.hpp"
#include "tui.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("RISC-V Simulator");
	program.add_argument("--elf", "-e").help("path to the ELF binary to load and simulate").required();
	program.add_argument("--headless", "-H").help("run without the TUI").default_value(false).implicit_value(true);

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception &e) {
		cerr << e.what() << endl;
		cerr << program.help().str();
		return 1;
	}

	string elfPath = program.get<string>("--elf");

	auto prog = Loader::ELF(elfPath);
	string asmPath = elfPath.substr(0, elfPath.size() - 4) + ".asm";
	auto disasm = Loader::ASM(asmPath);
	
	CPU cpu(prog);
	TUI tui(cpu, disasm);

	if (program.get<bool>("--headless"))
		while (cpu.running()) cpu.step();
	else tui.run();

	auto instr_count = cpu.getInstructionCount();
	auto cycle_count = cpu.getCycleCount();

	cout << "Instructions executed: " << instr_count << endl;
	cout << "Cycles taken: " << cycle_count << endl;
	cout << "IPC: " << fixed << setprecision(3) << static_cast<float>(instr_count) / max(cycle_count, 1) << endl;

	return 0;
};
