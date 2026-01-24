#pragma once
#include <ncurses.h>
#include <map>
#include <sstream>
#include "instruction.h"
#include "regfile.h"
#include "memory.h"

class TUI {
public:
	TUI() {
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		curs_set(0);
		
		if (has_colors()) {
			start_color();
			init_pair(1, COLOR_BLACK, COLOR_CYAN);    // highlight
			init_pair(2, COLOR_GREEN, COLOR_BLACK);   // labels
			init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // values
			init_pair(4, COLOR_RED, COLOR_BLACK);     // modified values
			init_pair(5, COLOR_WHITE, COLOR_BLUE);    // titles
		}
		
		getmaxyx(stdscr, maxY, maxX);
		createWindows();
		refresh();
	}
	
	~TUI() {
		if (instrWin) delwin(instrWin);
		if (regWin) delwin(regWin);
		if (memWin) delwin(memWin);
		endwin();
	}

	void renderFrame(uint32_t pc, const RegisterFile &regs, const Memory &mem, 
					 const map<uint32_t, string> &disasm) {
		renderInstructions(pc, disasm);
		renderRegisters(regs);
		renderMemory(mem, regs.read(2)); // pass sp (x2)
	}
	
	int waitForKey() { return getch(); }
	void displayMessage(const string &msg) {
		mvwprintw(stdscr, maxY - 1, 0, "%s", msg.c_str());
		wrefresh(stdscr);
	}

private:
	WINDOW* instrWin = nullptr;
	WINDOW* regWin = nullptr;
	WINDOW* memWin = nullptr;
	int maxY, maxX;
	vector<uint32_t> prevr = vector<uint32_t>(RegisterFile::NUM_REGISTERS, 0);
	map<uint32_t, uint32_t> prevm = map<uint32_t, uint32_t>();
	
	void createWindows() {
		int instrWidth = (maxX * 4) / 10;
		instrWin = newwin(maxY - 2, instrWidth, 0, 0);
		
		int regWidth = (maxX * 3) / 10;
		regWin = newwin(maxY - 2, regWidth, 0, instrWidth);
		
		int memWidth = maxX - instrWidth - regWidth;
		memWin = newwin(maxY - 2, memWidth, 0, instrWidth + regWidth);
	}
	
	void renderInstructions(uint32_t pc, const map<uint32_t, string> &disasm) {
		werase(instrWin);
		box(instrWin, 0, 0);
		
		if (has_colors()) wattron(instrWin, COLOR_PAIR(5) | A_BOLD);
		mvwprintw(instrWin, 0, 2, " Instructions ");
		if (has_colors()) wattroff(instrWin, COLOR_PAIR(5) | A_BOLD);
		
		int height, width;
		getmaxyx(instrWin, height, width);
		
		int visibleLines = height - 2;
		int centerLine = visibleLines / 2;

		auto pcIt = disasm.lower_bound(pc);
		auto startIt = pcIt;
		for (int i = 0; i < centerLine && startIt != disasm.begin(); i++) startIt--;
		
		int line = 1;
		for (auto displayIt = startIt; displayIt != disasm.end() && line < height - 1; displayIt++) {
			bool isCurrent = (displayIt == pcIt);
			
			if (isCurrent && has_colors()) wattron(instrWin, COLOR_PAIR(1) | A_BOLD);
			
			stringstream ss;
			ss << "0x" << hex << setw(8) << setfill('0') << displayIt->first << ": " << displayIt->second;
			
			string instrText = ss.str();
			if (instrText.length() > (size_t)(width - 4)) instrText = instrText.substr(0, width - 4);
			
			mvwprintw(instrWin, line++, 2, "%s", instrText.c_str());
			if (isCurrent && has_colors()) wattroff(instrWin, COLOR_PAIR(1) | A_BOLD);
		}
		
		wrefresh(instrWin);
	}
	
	void renderRegisters(const RegisterFile &regs) {
		werase(regWin);
		box(regWin, 0, 0);
		
		if (has_colors()) wattron(regWin, COLOR_PAIR(5) | A_BOLD);
		mvwprintw(regWin, 0, 2, " Registers ");
		if (has_colors()) wattroff(regWin, COLOR_PAIR(5) | A_BOLD);
		
		int height, width;
		getmaxyx(regWin, height, width);
		
		int line = 1;
		for (uint8_t i = 0; i < RegisterFile::NUM_REGISTERS && line < height - 1; i++) {
			uint32_t rv = regs.read(i);
			bool modified = (rv != prevr[i]);
			const char *name = regs.name(i);

			if (has_colors()) wattron(regWin, COLOR_PAIR(2));
			mvwprintw(regWin, line++, 2, "x%02d %s(%s):", i, strlen(name) < 3 ? " " : "", name);
			if (has_colors()) wattroff(regWin, COLOR_PAIR(2));
			
			if (modified && has_colors()) wattron(regWin, COLOR_PAIR(4) | A_BOLD);
			else if (has_colors()) wattron(regWin, COLOR_PAIR(3));
			wprintw(regWin, " 0x%08X", rv);
			
			if (modified && has_colors()) wattroff(regWin, COLOR_PAIR(4) | A_BOLD);
			else if (has_colors()) wattroff(regWin, COLOR_PAIR(3));
			
			prevr[i] = rv;
		}
		
		wrefresh(regWin);
	}
	
	void renderMemory(const Memory &mem, const uint32_t sp) {
		werase(memWin);
		box(memWin, 0, 0);
		
		if (has_colors()) wattron(memWin, COLOR_PAIR(5) | A_BOLD);
		mvwprintw(memWin, 0, 2, " Memory ");
		if (has_colors()) wattroff(memWin, COLOR_PAIR(5) | A_BOLD);
		
		int height, width;
		getmaxyx(memWin, height, width);

		int visibleLines = height - 2;
		
		int line = 1;
		uint32_t start = max(min(sp, uint32_t(CPU::MEM_SIZE) - visibleLines * 4), 0U);
		for (uint32_t addr = start; addr + 3 < CPU::MEM_SIZE && line < height - 1; addr += 4) {
			uint32_t word = mem.loadw(addr);
			bool modified = false;
			auto it = prevm.find(addr);
			if (it != prevm.end()) modified = (it->second != word);

			bool isSP = (addr == sp);

			if (modified && has_colors()) wattron(memWin, COLOR_PAIR(4) | A_BOLD);
			else if (isSP && has_colors()) wattron(memWin, COLOR_PAIR(2));
			else if (has_colors()) wattron(memWin, COLOR_PAIR(3));
			
			mvwprintw(memWin, line++, 2, "0x%08X: 0x%08X", addr, word);
			
			if (modified && has_colors()) wattroff(memWin, COLOR_PAIR(4) | A_BOLD);
			else if (isSP && has_colors()) wattroff(memWin, COLOR_PAIR(2));
			else if (has_colors()) wattroff(memWin, COLOR_PAIR(3));
			
			prevm[addr] = word;
		}
		
		wrefresh(memWin);
	}
};
