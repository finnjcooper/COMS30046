#include "tui.hpp"

using namespace ftxui;

Element TUI::themedWindow(const string &title, Element content) {
	return window(text(" " + title + " ") | bold | color(Theme::Text), content) 
		| color(Theme::Text) | bgcolor(Theme::BG);
}

void TUI::run() {
	auto screen = ScreenInteractive::Fullscreen();

	vector<string> tab_values = {"Registers", "Stack"};
	int tab_selected = 0;

	auto tab_renderer = Renderer([&] {
		Elements tabs;
		for (size_t i = 0; i < tab_values.size(); i++) {
			bool selected = (static_cast<int>(i) == tab_selected);
			Element tab = text(" " + tab_values[i] + " ");
			if (selected) tab |= bold | bgcolor(Theme::Accent) | color(Theme::BG);
			else tab |= color(Theme::Text) | dim;
			tabs.push_back(tab);
		}
		return hbox(tabs);
	});

	auto regs_comp = Renderer([&] { return renderRegisters(); });
	auto mem_comp = Renderer([&] { return renderMemory(); });
	auto tab_container = Container::Tab({regs_comp, mem_comp}, &tab_selected);
	
	auto right_renderer = Renderer(tab_container, [&] {
		return vbox({
			tab_renderer->Render() | align_right,
			tab_container->Render() | flex,
		});
	});

	auto left_renderer = Renderer([&] {
		return vbox({ text(""), renderInstructions() | flex });
	});

	auto bottom_renderer = Renderer([&] {
		return emptyElement();
		// return renderCPUStatus();
	});

	int left_size = 40;
	int bottom_size = 3;

	auto main_split = ResizableSplitLeft(left_renderer, right_renderer, &left_size);
	
	// auto full_layout = ResizableSplitBottom(bottom_renderer, main_split, &bottom_size);

	auto full_layout = CatchEvent(main_split, [&](Event event) {
		if (event == Event::Character('q') || event == Event::Escape) { screen.Exit(); return true; }
		if (event == Event::Character(' ') || event == Event::Return) { if (cpu.running()) cpu.step(); return true; }
		if (event == Event::Character('r')) { while (cpu.running()) cpu.step(); return true; }
		if (event == Event::Character('h')) { showHelp = !showHelp; return true; }
		if (event == Event::Tab) { tab_selected = (tab_selected + 1) % tab_values.size(); return true; }
		return false;
	});

	cpu.setStepCallback([&](const PipelineControl &ctrl, const CommitLog &log, const string &msg) {
		message = msg;
		isStalled = ctrl.stall;
		isFlushed = ctrl.flush;
		highlightedRegs.clear();
		for (const auto &rw : log.regWrites) highlightedRegs.insert(rw.reg);
		highlightedMem.clear();
		for (const auto &mw : log.memWrites) highlightedMem.insert(mw.addr);
	});

	auto layout = Renderer(full_layout, [&] {
		Element main = vbox({
			renderTitleBar(),
			full_layout->Render() | flex,
			renderCPUStatus(),
			renderStatusBar(),
		});
		
		return dbox({
			main | bgcolor(Theme::BG),
			showHelp ? renderHelpWindow() | center : emptyElement(),
		});
	});

	screen.Loop(layout);
}

Element TUI::renderTitleBar() {
	return text(" RISC-V Simulator ") | center | bold | bgcolor(Theme::Bar) | color(Theme::Text);
}

Element TUI::renderInstructions() {
	Elements lines;
	
	const auto &pipe = cpu.getPipeline();
	uint32_t pc = cpu.getPC();
	
	map<uint32_t, pair<char, Color>> stages;
	if (pipe.memwb.valid) stages[pipe.memwb.pc] = {'W', Theme::WB};
	if (pipe.exmem.valid) stages[pipe.exmem.pc] = {'M', Theme::MEM};
	if (pipe.idex.valid)  stages[pipe.idex.pc]  = {'X', Theme::EX};
	if (pipe.ifid.valid)  stages[pipe.ifid.pc]  = {'D', Theme::ID};
	stages[pc] = {'F', Theme::IF};
	
	for (const auto &[addr, orig_instr] : disasm) {
		stringstream ss;
		ss << hex << setw(8) << setfill('0') << addr;

		// render tabs as spaces
		string instr;
		int col = 0;
		for (char c : orig_instr) {
			if (c == '\t') {
				int spaces = 8 - (col % 8);
				instr.append(spaces, ' ');
				col += spaces;
			} else {
				instr += c;
				col++;
			}
		}

		auto it = stages.find(addr);
		auto [stage, stageColor] = (it != stages.end()) ? it->second : make_pair(' ', Theme::BG);

		Element stageBadge = text(string(" ") + stage + " ") | bold;
		if (it != stages.end()) stageBadge |= bgcolor(stageColor) | color(Theme::BG);

		Element line = hbox({
			stageBadge,
			text(" "),
			text("0x" + ss.str()) | color(Theme::Accent),
			text("  "),
			text(instr) | color(Theme::Text),
		});

		if (addr == pc) line |= focus;
		lines.push_back(line);
	}
	
	return themedWindow("Instructions", 
		vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 0.5f)
	);
}

Element TUI::renderRegisters() {
	Elements lines;
	const auto &regs = cpu.getRegisters();
	
	for (uint8_t i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
		uint32_t val = regs.read(i);
		string regname = regs.name(i);
		
		stringstream addr_ss, name_ss, val_ss;
		addr_ss << "x" << setw(2) << setfill('0') << static_cast<int>(i);
		name_ss << "(" << regname << ")";
		val_ss << "0x" << hex << setw(8) << setfill('0') << val;

		stringstream reg_ss;
		reg_ss << addr_ss.str() << setw(7) << setfill(' ') << name_ss.str();
		
		Element line = hbox({
			text(reg_ss.str()) | color(Theme::Accent),
			text("  "),
			text(val_ss.str()) | (highlightedRegs.count(i) ? color(Theme::RegsModified) | bold : color(Theme::Regs)),
		});
		
		lines.push_back(line);
	}
	
	return themedWindow("Registers", vbox(lines) | vscroll_indicator | frame);
}

Element TUI::renderMemory() {
	Elements lines;
	const auto &mem = cpu.getMemory();
	uint32_t sp = cpu.getRegisters().read(2);
	
	for (uint32_t addr = sp; addr < CPU::MEM_SIZE; addr += CPU::WORD_BYTES) {
		uint32_t word = mem.loadw(addr);
		bool isSP = (addr == sp);
		
		stringstream addr_ss, val_ss;
		addr_ss << "0x" << hex << setw(8) << setfill('0') << addr;
		val_ss << "0x" << hex << setw(8) << setfill('0') << word;
		
		Color valColour = Theme::Memory;
		if (highlightedMem.count(addr)) valColour = Theme::MemoryModified;
		else if (isSP) valColour = Theme::SP;
		
		Element line = hbox({
			text(addr_ss.str()) | color(Theme::Accent),
			text("  "),
			text(val_ss.str()) | color(valColour) | (highlightedMem.count(addr) || isSP ? bold : nothing),
			isSP ? (text("  ◄ sp") | color(Theme::SP) | bold) : emptyElement(),
		});
		
		if (isSP) line |= focus;
		lines.push_back(line);
	}
	
	return themedWindow("Stack", vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 1.0f));
}

Element TUI::renderCPUStatus() {
	float ipc = static_cast<float>(cpu.getInstructionCount()) / max(cpu.getCycleCount(), 1);
	
	stringstream pc_ss, cyc_ss, instr_ss, ipc_ss;
	pc_ss << "0x" << hex << setw(8) << setfill('0') << cpu.getPC();
	cyc_ss << cpu.getCycleCount();
	instr_ss << cpu.getInstructionCount();
	ipc_ss << fixed << setprecision(3) << ipc;
	
	Element status;
	if (!cpu.running()) {
		status = text(" HALTED ") | bgcolor(Theme::Red) | color(Theme::BG) | bold;
	} else if (isStalled) {
		status = text(" STALLED ") | bgcolor(Theme::Peach) | color(Theme::BG) | bold;
	} else if (isFlushed) {
		status = text(" FLUSHED ") | bgcolor(Theme::Mauve) | color(Theme::BG) | bold;
	} else {
		status = text(" RUNNING ") | bgcolor(Theme::Green) | color(Theme::BG) | bold;
	}
	
	return hbox({
		hbox({ text("PC: ") | color(Theme::Text), text(pc_ss.str()) | color(Theme::Mauve) | bold }),
		hbox({ text("  Cycles: ") | color(Theme::Text), text(cyc_ss.str()) | color(Theme::Yellow) }),
		hbox({ text("  Instrs: ") | color(Theme::Text), text(instr_ss.str()) | color(Theme::Yellow) }),
		hbox({ text("  IPC: ") | color(Theme::Text), text(ipc_ss.str()) | color(Theme::Green) | bold }),
		filler(),
		status,
	}) | bgcolor(Theme::BGLight);
}

Element TUI::renderStatusBar() {
	return hbox({
		text(" " + message) | flex,
		text(" [Space] Step  [r] Run  [h] Help  [q] Quit") | dim,
	}) | bgcolor(Theme::Bar) | color(Theme::BG);
}

Element TUI::renderHelpWindow() {
	auto keyRow = [](const string &key, const string &desc) {
		return hbox({
			text("  "),
			text(key) | bold | color(Theme::Accent) | size(WIDTH, EQUAL, 16),
			text(desc) | color(Theme::Text),
			text("  "),
		});
	};
	
	Element win = window(
		text(" Keyboard Shortcuts ") | bold | center | color(Theme::Text),
		vbox({
			text(""),
			keyRow("Space / Enter", "Step one cycle"),
			keyRow("r", "Run until halt"),
			keyRow("q / Esc", "Quit"),
			text(""),
			separatorLight(),
			text(""),
			keyRow("Tab", "Cycle right window tabs"),
			keyRow("h", "Toggle this help"),
			text(""),
		})
	) | bgcolor(Theme::BGLight) | size(WIDTH, EQUAL, 46) | clear_under | center;

	return win;
}
