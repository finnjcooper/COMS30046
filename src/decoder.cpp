#include "decoder.h"

Instruction Decoder::decode(uint32_t instruction) {
	uint8_t op = instruction & 0x7F;
	uint8_t rd = (instruction >> 7) & 0x1F;
	uint8_t rs1 = (instruction >> 15) & 0x1F;
	uint8_t rs2 = (instruction >> 20) & 0x1F;
	uint8_t f3 = (instruction >> 12) & 0x07;
	uint8_t f7 = (instruction >> 25) & 0x7F;

	cout << "Decoding instruction: " << setw(8) << setfill('0') << hex << instruction << " with opcode " << setw(2) << setfill('0') << hex << (int)op << dec << endl;

	switch (op) {
		case 0x33:
			if (f7 == 0x20) {
				if (f3 == 0x00) cout << "SUB" << endl;
				else if (f3 == 0x05) cout << "SRA" << endl;
			} else {
				if (f3 == 0x00) return Instruction {Opcode::ADD, rd, rs1, rs2, 0};
				else if (f3 == 0x01) cout << "SLL" << endl;
				else if (f3 == 0x02) cout << "SLT" << endl;
				else if (f3 == 0x03) cout << "SLTU" << endl;
				else if (f3 == 0x04) cout << "XOR" << endl;
				else if (f3 == 0x05) cout << "SRL" << endl;
				else if (f3 == 0x06) cout << "OR" << endl;
				else if (f3 == 0x07) cout << "AND" << endl;
			} break;
		case 0x13:
			if (f3 == 0x00) return Instruction {Opcode::ADDI, rd, rs1, 0, (instruction >> 20)};
			else if (f3 == 0x01) cout << "SLLI" << endl;
			else if (f3 == 0x02) cout << "SLTI" << endl;
			else if (f3 == 0x03) cout << "SLTIU" << endl;
			else if (f3 == 0x04) cout << "XORI" << endl;
			else if (f3 == 0x05) {
				if (f7 == 0x20) cout << "SRAI" << endl;
				else cout << "SRLI" << endl;
			}
			else if (f3 == 0x06) cout << "ORI" << endl;
			else if (f3 == 0x07) cout << "ANDI" << endl;
			break;
		case 0x03:
			cout << "Load" << endl;
			break;
		case 0x23:
			cout << "Store" << endl;
			break;
		case 0x63:
			cout << "Branch" << endl;
			break;
		case 0x6F:
			cout << "JAL" << endl;
			break;
		case 0x67:
			cout << "JALR" << endl;
			break;
		case 0x37:
			cout << "LUI" << endl;
			break;
		case 0x17:
			cout << "AUIPC" << endl;
			break;
		case 0x73:
			if (instruction >> 20 == 0x00) return Instruction {Opcode::ECALL, 0, 0, 0, 0};
			else if (instruction >> 20 == 0x01) return Instruction {Opcode::EBREAK, 0, 0, 0, 0};
		default:
			break;
	}

	return {Opcode::INVALID, 0, 0, 0};
}