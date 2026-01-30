#pragma once
#include <map>
#include <sstream>
#include <iomanip>
#include <functional>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "cpu.hpp"
#include "regfile.hpp"
#include "memory.hpp"
#include "pipeline.hpp"

using namespace ftxui;

class TUI {
public:
	TUI(CPU &cpu, const map<uint32_t, string> &disasm) : cpu(cpu), disasm(disasm) { prevRegs.fill(0); }

	void run() {
		auto screen = ScreenInteractive::Fullscreen();

		auto component = Renderer([&] {
			return hbox({
				renderInstructions() | flex,
				renderRegisters() | flex,
				renderMemory() | flex,
			}) | flex;
		});

		component = CatchEvent(component, [&](Event event) {
			if (event == Event::Character('q') || event == Event::Escape) { screen.Exit(); return true; }
			if (event == Event::Character(' ') || event == Event::Return) { if (cpu.running()) cpu.step(); return true; }
			if (event == Event::Character('r')) { while (cpu.running()) cpu.step(); return true; }
			if (event == Event::Character('h')) { showHelp = !showHelp; return true; }
			return false;
		});

		auto layout = Renderer(component, [&] {
			Elements content;
			content.push_back(renderTitleBar());
			content.push_back(component->Render() | flex);
			content.push_back(renderStatusBar());
			
			if (showHelp) {
				auto helpBox = renderHelpWindow() | center;
				return dbox({
					vbox(content),
					helpBox,
				});
			}

			return vbox(content);
		});

		screen.Loop(layout);
	}

private:
	CPU &cpu;
	const map<uint32_t, string> &disasm;
	
	array<uint32_t, RegisterFile::NUM_REGISTERS> prevRegs;
	map<uint32_t, uint32_t> prevMem;
	bool showHelp = false;

	Element renderTitleBar() {
		return text(" RISC-V Simulator TUI ") | bold | center | bgcolor(Color::Blue) | color(Color::White);
	}

	Element renderInstructions() {
		Elements lines;
		
		const auto &pipe = cpu.getPipeline();
		uint32_t pc = cpu.getPC();
		
		map<uint32_t, pair<char, Color>> stages;
		if (pipe.memwb.valid) stages[pipe.memwb.pc] = {'W', Color::Green};
		if (pipe.exmem.valid) stages[pipe.exmem.pc] = {'M', Color::GrayLight};
		if (pipe.idex.valid)  stages[pipe.idex.pc]  = {'X', Color::Yellow};
		if (pipe.ifid.valid)  stages[pipe.ifid.pc]  = {'D', Color::Magenta};
		stages[pc] = {'F', Color::Cyan};
		
		for (const auto &[addr, instr] : disasm) {
			stringstream ss;
			ss << hex << setw(8) << setfill('0') << addr;
			
			auto it = stages.find(addr);
			bool isStage = (it != stages.end());
			char stage = isStage ? it->second.first : ' ';
			Color stageColor = isStage ? it->second.second : Color::White;
			
			Element line = hbox({
				text(string(1, stage)) | color(stageColor) | bold,
				text(" 0x" + ss.str() + ": "),
				text(instr),
			});
			
			if (isStage) line = line | bgcolor(stageColor) | color(Color::Black);
			if (addr == pc) line = line | focus;
			
			lines.push_back(line);
		}
		
		return window(
			text(" Instructions ") | bold,
			vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 0.5f)
		);
	}

	Element renderRegisters() {
		Elements lines;
		const auto &regs = cpu.getRegisters();
		
		for (uint8_t i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
			uint32_t val = regs.read(i);
			bool modified = (val != prevRegs[i]);
			prevRegs[i] = val;
			
			stringstream ss;
			ss << "x" << setw(2) << setfill('0') << static_cast<int>(i)
			   << " (" << setw(4) << left << setfill(' ') << regs.name(i) << "): "
			   << "0x" << hex << setw(8) << setfill('0') << right << val;
			
			Element line = text(ss.str());
			if (modified) line = line | color(Color::Red) | bold;
			else line = line | color(Color::Green);
			lines.push_back(line);
		}
		
		return window(text(" Registers ") | bold, vbox(lines) | vscroll_indicator | frame);
	}

	Element renderMemory() {
		Elements lines;
		const auto &mem = cpu.getMemory();
		uint32_t sp = cpu.getRegisters().read(2);
		
		for (uint32_t addr = sp; addr < CPU::MEM_SIZE; addr += CPU::WORD_BYTES) {
			uint32_t word = mem.loadw(addr);
			
			bool modified = false;
			auto it = prevMem.find(addr);
			if (it != prevMem.end()) modified = (it->second != word);
			prevMem[addr] = word;
			
			bool isSP = (addr == sp);
			
			stringstream ss;
			ss << "0x" << hex << setw(8) << setfill('0') << addr << ": "
			   << "0x" << setw(8) << setfill('0') << word;
			if (isSP) ss << " <sp";
			
			Element line = text(ss.str());
			if (modified) line = line | color(Color::Red) | bold;
			else if (isSP) line = line | color(Color::Green) | bold | focus;
			else line = line | color(Color::Yellow);
			
			lines.push_back(line);
		}
		
		return window(
			text(" Memory ") | bold,
			vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 1.0f)
		);
	}

	Element renderStatusBar() {
		float ipc = static_cast<float>(cpu.getNumInstructions()) / max(cpu.getNumCycles(), 1);
		
		stringstream ss;
		ss << " Cycle: " << cpu.getNumCycles()
		   << " | Instructions: " << cpu.getNumInstructions()
		   << " | IPC: " << fixed << setprecision(2) << ipc
		   << " | PC: 0x" << hex << setw(8) << setfill('0') << cpu.getPC()
		   << " | [h]elp [q]uit";
		
		return text(ss.str()) | bgcolor(Color::Blue) | color(Color::White);
	}

	Element renderHelpWindow() {
		return window(
			text(" Help ") | bold | center,
			vbox({
				text("                                      "),
				text("  Space / Enter   Step one cycle      "),
				text("  r               Run until halt      "),
				text("  q / Esc         Quit                "),
				text("                                      "),
				text("  h               Toggle this help    "),
				text("                                      "),
				text("  Pipeline: F=Fetch   D=Decode        "),
				text("            X=Execute M=Mem W=WB      "),
				text("                                      "),
			})
		) | bgcolor(Color::Red) | color(Color::White) | size(WIDTH, EQUAL, 42) | size(HEIGHT, EQUAL, 18);
	}
};
