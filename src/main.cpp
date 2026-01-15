#include "cpu.h"
#include "loader.h"
#include "tui.h"

int main(int argc, char* argv[]) {
	auto prog = Loader::ELF(argv[1]);
	auto disasm = Loader::ASM("build/bench.asm");

	CPU cpu(prog);
	TUI tui;

	tui.renderFrame(cpu.getPC(), cpu.getRegisters(), cpu.getMemory(), disasm);
	tui.displayMessage("Press any key to step, 'c' to continue, 'q' to quit");
	
	bool stepping = true;
	while (cpu.isRunning()) {
		if (stepping) {
			int ch = tui.waitForKey();
			if (ch == 'q' || ch == 'Q') break;
			if (ch == 'c' || ch == 'C') stepping = false;
			else stepping = true;
		}
		
		cpu.step();
		tui.renderFrame(cpu.getPC(), cpu.getRegisters(), cpu.getMemory(), disasm);
		cpu.commitMemory();
	}

	tui.displayMessage("Program halted. Press any key to exit.                     ");
	tui.waitForKey();

	return 0;
};
