#include "cpu.h"

int main(int argc, char* argv[]) {
	auto prog = Loader::fromElf(argv[1]);

	CPU cpu(prog);
	// cpu.imem.print32();

	cout << endl << "Starting CPU... Press enter to step." << endl;
	
	while (cpu.isRunning()) {
		// cin.get();
		cpu.step();
	}

	for (int i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
		cout << "x" << i << ": " << cpu.regs.read(i) << " ";;
	}

	return 0;
}