#include "tui.hpp"

static int clamp_index(int index, int item_count) {
	if (item_count <= 0) return 0;
	if (index < 0) return 0;
	if (index >= item_count) return item_count - 1;
	return index;
}

static int stack_line_count(const CPU &cpu) {
	const uint32_t sp = cpu.get_registers().read(2).as_scalar();
	if (sp >= MEM_SIZE) return 0;
	const uint32_t remaining = MEM_SIZE - sp;
	return static_cast<int>((remaining + WORD_BYTES - 1) / WORD_BYTES);
}

static bool scroll_on_hover(Event event, const Box &panel_box, int item_count, int &focus_index) {
	if (!event.is_mouse() || item_count <= 0) return false;
	if (!panel_box.Contain(event.mouse().x, event.mouse().y)) return false;
	if (event.mouse().button != Mouse::WheelUp && event.mouse().button != Mouse::WheelDown) return false;

	focus_index += (event.mouse().button == Mouse::WheelDown) ? 1 : -1;
	focus_index = clamp_index(focus_index, item_count);
	return true;
}

void TUI::halt() {
	screen.Exit();
}

void TUI::run() {
	vector<string> tab_values = {"Registers", "Stack"};
	int tab_selected = 0;
	Box left_panel_box;
	Box right_panel_box;

	auto sync_disasm_focus_with_pc = [&] {
		int idx = 0;
		for (auto it = disasm.begin(); it != disasm.end(); ++it, ++idx) {
			if (it->first == cpu.get_pc()) {
				disasm_focus = idx;
				return;
			}
		}
	};
	sync_disasm_focus_with_pc();

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

	auto regs_comp = Renderer([&] { return render_registers(); });
	auto mem_comp = Renderer([&] { return render_memory(); });
	auto tab_container = Container::Tab({regs_comp, mem_comp}, &tab_selected);
	
	auto right_renderer = Renderer(tab_container, [&] {
		return vbox({
			tab_renderer->Render() | align_right,
			tab_container->Render() | flex,
		}) | reflect(right_panel_box);
	});

	auto left_renderer = Renderer([&] {
		return vbox({ text(""), render_instructions() | flex });
	}) | reflect(left_panel_box);

	auto bottom_renderer = Renderer([&] {
		return emptyElement();
		// return render_cpu_status();
	});

	int left_size = 40;
	int bottom_size = 3;

	auto main_split = ResizableSplitLeft(left_renderer, right_renderer, &left_size);
	
	// auto full_layout = ResizableSplitBottom(bottom_renderer, main_split, &bottom_size);

	auto full_layout = CatchEvent(main_split, [&](Event event) {
		if (scroll_on_hover(event, left_panel_box, static_cast<int>(disasm.size()), disasm_focus)) return true;

		const int right_item_count = (tab_selected == 0)
			? static_cast<int>(NUM_REGISTERS)
			: stack_line_count(cpu);
		int &right_focus = (tab_selected == 0) ? regs_focus : stack_focus;
		if (scroll_on_hover(event, right_panel_box, right_item_count, right_focus)) return true;

		if (event == Event::Character('q') || event == Event::Escape) { screen.Exit(); return true; }
		if (event == Event::Character(' ') || event == Event::Return) { if (cpu.running()) cpu.step(); return true; }
		if (event == Event::Character('r')) { while (cpu.running()) cpu.step(); return true; }
		if (event == Event::Character('h')) { show_help = !show_help; return true; }
		if (event == Event::Tab) { tab_selected = (tab_selected + 1) % tab_values.size(); return true; }
		return false;
	});

	cpu.set_step_callback([&](bool jmp, bool stall, const CommitLog &log, const string &msg) {
		message = msg;
		flushed = jmp;
		stalled = stall;
		sync_disasm_focus_with_pc();
		highlighted_regs.clear();
		for (const auto &rw : log.reg_writes) highlighted_regs.insert(rw.reg);
		highlighted_mem.clear();
		for (const auto &mw : log.mem_writes) highlighted_mem.insert(mw.addr);
	});

	auto layout = Renderer(full_layout, [&] {
		Element main = vbox({
			render_title_bar(),
			full_layout->Render() | flex,
			render_cpu_status(),
			render_message_bar(),
		});
		
		return dbox({
			main | bgcolor(Theme::BG),
			show_help ? render_help_window() | center : emptyElement(),
		});
	});

	screen.Loop(layout);
}

Element TUI::themed_window(const string &title, Element content) {
	return window(text(" " + title + " ") | bold | color(Theme::Text), content) 
		| color(Theme::Text) | bgcolor(Theme::BG);
}

Element TUI::render_title_bar() {
	return text(" RISC-V Simulator ") | center | bold | bgcolor(Theme::Bar) | color(Theme::BG);
}

Element TUI::render_instructions() {
	Elements lines;
	disasm_focus = clamp_index(disasm_focus, static_cast<int>(disasm.size()));
	int line_index = 0;

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

		Element line = hbox({
			text("0x" + ss.str()) | color(Theme::Accent),
			text("  "),
			text(instr) | color(Theme::Text),
		});
		if (line_index == disasm_focus) line |= focus | bgcolor(Theme::BGLight);

		lines.push_back(line);
		line_index++;
	}
	
	return themed_window("Instructions", 
		vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 0.5f)
	);
}

Element TUI::render_registers() {
	Elements lines;
	const auto &regs = cpu.get_registers();
	regs_focus = clamp_index(regs_focus, static_cast<int>(NUM_REGISTERS));
	
	for (uint8_t i = 0; i < NUM_REGISTERS; i++) {
		uint32_t val = regs.read(i).as_scalar();
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
			text(val_ss.str()) | (highlighted_regs.count(i) ? color(Theme::RegsModified) | bold : color(Theme::Regs)),
		});
		if (static_cast<int>(i) == regs_focus) line |= focus | bgcolor(Theme::BGLight);
		
		lines.push_back(line);
	}
	
	return themed_window("Registers", vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 0.0f));
}

Element TUI::render_memory() {
	Elements lines;
	const auto &mem = cpu.get_memory();
	uint32_t sp = cpu.get_registers().read(2).as_scalar();
	stack_focus = clamp_index(stack_focus, stack_line_count(cpu));
	int line_index = 0;
	
	for (uint32_t addr = sp; addr < MEM_SIZE; addr += WORD_BYTES) {
		uint32_t word = mem.loadw(addr);
		bool isSP = (addr == sp);
		
		stringstream addr_ss, val_ss;
		addr_ss << "0x" << hex << setw(8) << setfill('0') << addr;
		val_ss << "0x" << hex << setw(8) << setfill('0') << word;
		
		Color valColour = Theme::Memory;
		if (highlighted_mem.count(addr)) valColour = Theme::MemoryModified;
		else if (isSP) valColour = Theme::SP;
		
		Element line = hbox({
			text(addr_ss.str()) | color(Theme::Accent),
			text("  "),
			text(val_ss.str()) | color(valColour) | (highlighted_mem.count(addr) || isSP ? bold : nothing),
			isSP ? (text("  ◄ sp") | color(Theme::SP) | bold) : emptyElement(),
		});

		if (line_index == stack_focus) line |= focus | bgcolor(Theme::BGLight);
		lines.push_back(line);
		line_index++;
	}
	
	return themed_window("Stack", vbox(lines) | vscroll_indicator | frame | focusPositionRelative(0.0f, 0.0f));
}

Element TUI::render_cpu_status() {
	float ipc = static_cast<float>(cpu.get_instruction_count()) / max(cpu.get_cycle_count(), 1);
	
	stringstream pc_ss, cyc_ss, instr_ss, ipc_ss;
	pc_ss << "0x" << hex << setw(8) << setfill('0') << cpu.get_pc();
	cyc_ss << cpu.get_cycle_count();
	instr_ss << cpu.get_instruction_count();
	ipc_ss << fixed << setprecision(3) << ipc;
	
	Element status;
	if (!cpu.running()) {
		status = text(" HALTED ") | bgcolor(Theme::Red) | color(Theme::BG) | bold;
	} else if (stalled) {
		status = text(" STALLED ") | bgcolor(Theme::Peach) | color(Theme::BG) | bold;
	} else if (flushed) {
		status = text(" FLUSHED ") | bgcolor(Theme::Mauve) | color(Theme::BG) | bold;
	} else {
		status = text(" RUNNING ") | bgcolor(Theme::Green) | color(Theme::BG) | bold;
	}
	
	return hbox({
		hbox({ text(" PC: ") | color(Theme::Text), text(pc_ss.str()) | color(Theme::Mauve) }),
		hbox({ text("  Cycles: ") | color(Theme::Text), text(cyc_ss.str()) | color(Theme::Yellow) }),
		hbox({ text("  Instrs: ") | color(Theme::Text), text(instr_ss.str()) | color(Theme::Yellow) }),
		hbox({ text("  IPC: ") | color(Theme::Text), text(ipc_ss.str()) | color(Theme::Green) }),
		filler(),
		status,
	}) | bgcolor(Theme::BGLight);
}

Element TUI::render_message_bar() {
	return hbox({
		text(" " + message) | flex,
		text(" [Space] Step  [r] Run  [h] Help  [q] Quit "),
	}) | bgcolor(Theme::Bar) | color(Theme::BG);
}

Element TUI::render_help_window() {
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
			keyRow("Mouse wheel", "Scroll hovered panel"),
			keyRow("Tab", "Cycle right window tabs"),
			keyRow("h", "Toggle this help"),
			text(""),
		})
	) | bgcolor(Theme::BGLight) | size(WIDTH, EQUAL, 46) | clear_under | center;

	return win;
}
