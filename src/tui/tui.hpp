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
	const Color BGLight = Color(0x31, 0x32, 0x44);
	const Color BG = Color(0x11, 0x11, 0x1e);
	const Color Text = Color(0xcd, 0xd6, 0xf4);

	const Color Mauve = Color(0xcb, 0xa6, 0xf7);
	const Color Red = Color(0xf3, 0x8b, 0xa8);
	const Color Peach = Color(0xfa, 0xb3, 0x87);
	const Color Green = Color(0xa6, 0xe3, 0xa1);
	const Color Blue = Color(0x89, 0xb4, 0xfa);
	const Color Yellow = Color(0xf9, 0xe2, 0xaf);

	const Color Accent = Mauve;
	const Color Bar = Blue;
	
	const Color IF = Mauve;
	const Color ID = Red;
	const Color EX = Peach;
	const Color MEM = Green;
	const Color WB = Blue;

	const Color Regs = Yellow;
	const Color RegsModified = Red;
	const Color Memory = Yellow;
	const Color MemoryModified = Red;
	const Color SP = Peach;
}


class TUI {
public:
	TUI(CPU &cpu, const map<uint32_t, string> &disasm) : cpu(cpu), disasm(disasm) {}
	~TUI() { halt(); }

	void run();
	void halt();

private:
	CPU &cpu;
	const map<uint32_t, string> &disasm;
	ScreenInteractive screen = ScreenInteractive::Fullscreen();
	
	string message = "";
	set<uint8_t> highlighted_regs;
	set<uint32_t> highlighted_mem;
	int disasm_focus = 0;
	int regs_focus = 0;
	int stack_focus = 0;
	bool show_help = false;
	bool stalled = false;
	bool flushed = false;

	Element render_title_bar();
	Element render_instructions();
	Element render_registers();
	Element render_memory();
	Element render_cpu_status();
	Element render_message_bar();
	Element render_help_window();
	
	Element themed_window(const string &title, Element content);
};
