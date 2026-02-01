#pragma once
#include <map>
#include <set>
#include <string>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "cpu.hpp"

using namespace ftxui;
using namespace std;

// catppuccin mocha
namespace Theme {
	const Color BG = Color(0x1e, 0x1e, 0x2e);
	const Color Text = Color(0xcd, 0xd6, 0xf4);

	const Color Mauve = Color(0xcb, 0xa6, 0xf7);
	const Color Red = Color(0xf3, 0x8b, 0xa8);
	const Color Peach = Color(0xfa, 0xb3, 0x87);
	const Color Green = Color(0xa6, 0xe3, 0xa1);
	const Color Blue = Color(0x89, 0xb4, 0xfa);
	const Color Yellow = Color(0xf9, 0xe2, 0xaf);

	const Color Bar = Blue;
	
	const Color IF = Mauve;
	const Color ID = Red;
	const Color EX = Peach;
	const Color MEM = Green;
	const Color WB = Blue;

	const Color Regs = Green;
	const Color RegsModified = Red;
	const Color Memory = Yellow;
	const Color MemoryModified = Red;
	const Color SP = Peach;
	
	const Color HelpBG = Red;
}


class TUI {
public:
	TUI(CPU &cpu, const map<uint32_t, string> &disasm) : cpu(cpu), disasm(disasm) {}

	void run();

private:
	CPU &cpu;
	const map<uint32_t, string> &disasm;
	
	string message = "";
	set<uint8_t> highlightedRegs;
	set<uint32_t> highlightedMem;
	bool showHelp = false;

	Element renderTitleBar();
	Element renderBackground();
	Element renderInstructions();
	Element renderRegisters();
	Element renderMemory();
	Element renderStatusBar();
	Element renderHelpWindow();
};
