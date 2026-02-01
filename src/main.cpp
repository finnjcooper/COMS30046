#include "loader.hpp"
#include "tui.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("RISC-V Simulator");
	program.add_argument("--elf", "-e").help("path to the ELF binary to load and simulate").required();
	program.add_argument("--pipelined", "-p").help("enable pipelined execution").flag();
	program.add_argument("--forwarding", "-f").help("enable data forwarding").flag();

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception &e) {
		cerr << e.what() << endl;
		cerr << program.help().str();
		return 1;
	}

	string elfPath = program.get<string>("--elf");
	bool isPipelined = program.get<bool>("--pipelined");
	bool isForwarding = program.get<bool>("--forwarding");

	auto prog = Loader::ELF(elfPath);
	string asmPath = elfPath.substr(0, elfPath.size() - 4) + ".asm";
	auto disasm = Loader::ASM(asmPath);
	
	CPU cpu(prog, isPipelined, isForwarding);
	TUI tui(cpu, disasm);
	tui.run();

	cout << "Instructions executed: " << cpu.getInstructionCount() << endl;
	cout << "Cycles taken: " << cpu.getCycleCount() << endl;

	return 0;
};
