#include "cpu.h"

int main(int argc, char* argv[]) {
	vector<uint8_t> raw = Loader::fromBinary(argv[1]);

	CPU cpu(raw, {});

	cout << endl << "Starting CPU... Press enter to step." << endl;
	
	while (cpu.isRunning()) {
		// cin.get();
		cpu.step();
		// for (int i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
		// 	cout << "x" << i << ": " << hex << cpu.regs.read(i) << dec << " ";
		// }
	}

	return 0;
}