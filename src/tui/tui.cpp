#include "tui.hpp"

using namespace ftxui;

void TUI::run() {
	auto screen = ScreenInteractive::Fullscreen();

	vector<string> tab_values = {" Registers ", " Memory "};
	int tab_selected = 0;
	auto tab_toggle = Toggle(&tab_values, &tab_selected);

	auto regs_comp = Renderer([&] { return renderRegisters(); });
	auto mem_comp = Renderer([&] { return renderMemory(); });
	auto tab_container = Container::Tab({regs_comp, mem_comp}, &tab_selected);
	auto right_container = Container::Vertical({tab_toggle, tab_container});
	
	auto right_renderer = Renderer(right_container, [&] {
		return vbox({ tab_toggle->Render(), tab_container->Render() });
	});

	auto left_renderer = Renderer([&] {
		return vbox({ text(""), renderInstructions() });
	});

	int left_size = 50;

	auto component = right_renderer;
	component = ResizableSplitLeft(left_renderer, component, &left_size);

	component = CatchEvent(component, [&](Event event) {
		if (event == Event::Character('q') || event == Event::Escape) { screen.Exit(); return true; }
		if (event == Event::Character(' ') || event == Event::Return) { if (cpu.running()) cpu.step(); return true; }
		if (event == Event::Character('r')) { while (cpu.running()) cpu.step(); return true; }
		if (event == Event::Character('h')) { showHelp = !showHelp; return true; }
		return false;
	});

	cpu.setStepCallback([&](const CommitLog &log, const string &msg) {
		message = msg;
		highlightedRegs.clear();
		for (const auto &rw : log.regWrites) highlightedRegs.insert(rw.reg);
		highlightedMem.clear();
		for (const auto &mw : log.memWrites) highlightedMem.insert(mw.addr);
	});

	auto layout = Renderer(component, [&] {
		Elements content;
		content.push_back(renderTitleBar());
		content.push_back(component->Render() | flex);
		content.push_back(renderStatusBar());
		
		if (showHelp) {
			auto helpBox = renderHelpWindow() | center;
			return dbox({
				renderBackground(),
				vbox(content),
				helpBox,
			});
		}

		return dbox({ renderBackground(), vbox(content) });
	});

	screen.Loop(layout);
}

Element TUI::renderTitleBar() {
	return text(" RISC-V Simulator TUI ") | bold | center | bgcolor(Theme::Bar) | color(Theme::Text);
}

Element TUI::renderBackground() {
	return text("") | flex | bgcolor(Theme::BG);
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
		size_t i = 0;
		while (i < orig_instr.size()) {
			if (orig_instr[i] == '\t') {
				// find preceding word
				size_t j = i;
				while (j > 0 && orig_instr[j-1] != ' ' && orig_instr[j-1] != '\t') --j;
				size_t wordlen = i - j;
				size_t nspaces = (wordlen < 7) ? (7 - wordlen) : 1;
				instr.append(nspaces, ' ');
				++i;
			} else {
				instr += orig_instr[i++];
			}
		}

		auto it = stages.find(addr);
		auto [stage, stageColor] = (it != stages.end()) ? it->second : make_pair(' ', Color::Default);

		Element line = hbox({
			text(string(1, stage)) | bold,
			text(" 0x" + ss.str() + ": "),
			text(instr),
		});

		line = line | bgcolor(stageColor) | color(Theme::Text);
		if (addr == pc) line = line | focus;

		lines.push_back(line);
	}
	
	return window(
		text(" Instructions ") | bold,
		vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 0.5f)
	);
}

Element TUI::renderRegisters() {
	Elements lines;
	const auto &regs = cpu.getRegisters();
	
	for (uint8_t i = 0; i < RegisterFile::NUM_REGISTERS; i++) {
		uint32_t val = regs.read(i);
		string regname = regs.name(i);
		
		stringstream ss;
		ss << "x" << setw(2) << setfill('0') << static_cast<int>(i);

		stringstream name_ss;
		name_ss << "(" << regname << ")";
		string name = name_ss.str();
		int pad = 7 - static_cast<int>(name.length());

		ss << string(pad, ' ') << name << ": "
		   << "0x" << hex << setw(8) << setfill('0') << right << val;
		
		Element line = text(ss.str());
		if (highlightedRegs.count(i)) line = line | color(Theme::RegsModified) | bold;
		else line = line | color(Theme::Regs);
		lines.push_back(line);
	}
	
	return vbox(lines) | vscroll_indicator | frame | borderRounded;
}

Element TUI::renderMemory() {
	Elements lines;
	const auto &mem = cpu.getMemory();
	uint32_t sp = cpu.getRegisters().read(2);
	
	for (uint32_t addr = sp; addr < CPU::MEM_SIZE; addr += CPU::WORD_BYTES) {
		uint32_t word = mem.loadw(addr);
		bool isSP = (addr == sp);
		
		stringstream ss;
		ss << "0x" << hex << setw(8) << setfill('0') << addr << ": "
		   << "0x" << setw(8) << setfill('0') << word;
		if (isSP) ss << right << " < sp";
		
		Element line = text(ss.str());
		if (highlightedMem.count(addr)) line = line | color(Theme::MemoryModified) | bold;
		else if (isSP) line = line | color(Theme::SP) | bold | focus;
		else line = line | color(Theme::Memory);
		
		lines.push_back(line);
	}
	
	return vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 1.0f) | borderRounded;
}

Element TUI::renderStatusBar() {
	float ipc = static_cast<float>(cpu.getInstructionCount()) / max(cpu.getCycleCount(), 1);
	
	stringstream ss;
	ss << " Cycle: " << cpu.getCycleCount()
	   << " | Instructions: " << cpu.getInstructionCount()
	   << " | IPC: " << fixed << setprecision(3) << ipc
	   << " | PC: 0x" << hex << setw(8) << setfill('0') << cpu.getPC()
	   << " | [h]elp [q]uit";
	
	return vbox({ text(" " + message), text(ss.str()) }) | bgcolor(Theme::Bar) | color(Theme::Text);
}

Element TUI::renderHelpWindow() {
	Element win = window(
		text(" Help ") | bold | center,
		vbox({
			text("                                    "),
			text("  Space / Enter   Step one cycle    "),
			text("  r               Run until halt    "),
			text("  q / Esc         Quit              "),
			text("                                    "),
			text("  Tab             Cycle tabs        "),
			text("                                    "),
			text("  h               Toggle this help  "),
			text("                                    "),
		})
	) | color(Theme::Text) | bgcolor(Theme::HelpBG) | flex | clear_under | center;

	return win;
}
