#include "cpu.h"
#include "loader.h"
#include "tui.h"

struct Args {
	string elfPath;
	bool pipelined = true;
};

Args parseArgs(int argc, char* argv[]) {
	Args args;

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <program.elf> [pipelined(true/false)=true]" << endl;
		exit(1);
	}

	args.elfPath = argv[1];

	if (argc == 3) {
		string pipelinedStr = argv[2];
		if (pipelinedStr != "true" && pipelinedStr != "false") {
			cerr << "Invalid pipeline mode. Use true or false." << endl;
			exit(1);
		} else {
			args.pipelined = (pipelinedStr == "true");
		}
	}

	return args;
}

int main(int argc, char* argv[]) {
	auto [elfPath, isPipelined] = parseArgs(argc, argv);

	auto prog = Loader::ELF(elfPath);
	string asmPath = elfPath;
	asmPath = asmPath.substr(0, asmPath.size() - 4) + ".asm";
	auto disasm = Loader::ASM(asmPath);
	
	CPU cpu(prog, isPipelined);
	TUI tui;

	tui.renderFrame(cpu.getPipeline(), cpu.getRegisters(), cpu.getMemory(), disasm);
	tui.displayMessage("Press any key to step, 'c' to continue, 'q' to quit.");
	
	int ch;
	bool stepping = true;
	while (cpu.running()) {
		if (stepping) {
			ch = tui.waitForKey();
			if (ch == 'q' || ch == 'Q') break;
			if (ch == 'c' || ch == 'C') stepping = false;
			else stepping = true;
		}
		
		cpu.step();
		tui.renderFrame(cpu.getPipeline(), cpu.getRegisters(), cpu.getMemory(), disasm);
		tui.displayMessage(cpu.readout());
	}

	tui.displayMessage("Program halted. Press 'q' to quit.");
	while (ch != 'q' && ch != 'Q') ch = tui.waitForKey();

	cout << "Instructions executed: " << cpu.getNumInstructions() << endl;
	cout << "Cycles taken: " << cpu.getNumCycles() << endl;

	return 0;
};
