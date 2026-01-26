#include "gui.hpp"
#include "c8ke.hpp"


void checkError(bool cond, std::string msg) {
	if (cond) {
		std::cerr << "[c8ke] " << msg << std::endl;
		exit(EXIT_FAILURE);
	}
}

SDL_Keycode GUI::findSDLKeycode(uint8_t chip8Key) {
	for (const auto& [keycode, val] : keymap) {
		if (val == chip8Key) return keycode;
	}
	return SDLK_UNKNOWN;
}

//static void* Settings_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name) {
//	if (strcmp(name, "Colors") == 0)
//		return &customColors;
//	if (strcmp(name, "Audio") == 0)
//		return &customAudio;
//	return nullptr;
//}
//
//static void Settings_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* user_data, const char* line) {
//	CustomColors* colorSettings = (CustomColors*)user_data;
//	float r, g, b, a;
//
//	if (sscanf_s(line, "emuFg=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->emuFg = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "emuBg=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->emuBg = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "dbgColor1=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->dbgColor1 = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "dbgColor2=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->dbgColor2 = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "dbgColor3=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->dbgColor3 = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "dbgBg=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->dbgBg = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "dbgHeaderFg=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->dbgHeaderFg = ImVec4(r, g, b, a);
//	else if (sscanf_s(line, "dbgHeaderBg=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
//		colorSettings->dbgHeaderBg = ImVec4(r, g, b, a);
//
//	CustomAudio* audioSettings = (CustomAudio*)user_data;
//	int val;
//
//	if (sscanf_s(line, "beepAmount=%d", &val) == 1)
//		audioSettings->beepAmount = val;
//	else if (sscanf_s(line, "beepPhase=%d", &val) == 1)
//		audioSettings->beepPhase = val;
//
//}
//
//static void Settings_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf) {
//	const CustomColors& c = customColors;
//	out_buf->appendf("[%s][Colors]\n", handler->TypeName);
//	out_buf->appendf("emuFg=%.3f,%.3f,%.3f,%.3f\n", c.emuFg.x, c.emuFg.y, c.emuFg.z, c.emuFg.w);
//	out_buf->appendf("emuBg=%.3f,%.3f,%.3f,%.3f\n", c.emuBg.x, c.emuBg.y, c.emuBg.z, c.emuBg.w);
//	out_buf->appendf("dbgColor1=%.3f,%.3f,%.3f,%.3f\n", c.dbgColor1.x, c.dbgColor1.y, c.dbgColor1.z, c.dbgColor1.w);
//	out_buf->appendf("dbgColor2=%.3f,%.3f,%.3f,%.3f\n", c.dbgColor2.x, c.dbgColor2.y, c.dbgColor2.z, c.dbgColor2.w);
//	out_buf->appendf("dbgColor3=%.3f,%.3f,%.3f,%.3f\n", c.dbgColor3.x, c.dbgColor3.y, c.dbgColor3.z, c.dbgColor3.w);
//	out_buf->appendf("dbgBg=%.3f,%.3f,%.3f,%.3f\n", c.dbgBg.x, c.dbgBg.y, c.dbgBg.z, c.dbgBg.w);
//	out_buf->appendf("dbgHeaderFg=%.3f,%.3f,%.3f,%.3f\n", c.dbgHeaderFg.x, c.dbgHeaderFg.y, c.dbgHeaderFg.z, c.dbgHeaderFg.w);
//	out_buf->appendf("dbgHeaderBg=%.3f,%.3f,%.3f,%.3f\n", c.dbgHeaderBg.x, c.dbgHeaderBg.y, c.dbgHeaderBg.z, c.dbgHeaderBg.w);
//
//	out_buf->appendf("\n");
//
//	const CustomAudio& a = customAudio;
//	out_buf->appendf("[%s][Audio]\n", handler->TypeName);
//	out_buf->appendf("beepAmount=%d\n", a.beepAmount);
//	out_buf->appendf("beepPhase=%d\n", a.beepPhase);
//}

void GUI::initializeGui() {
	/* SDL */
	checkError(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), SDL_GetError());
	// main window
	window = SDL_CreateWindow("c8ke", windowWidth, windowHeight, SDL_WINDOW_HIGH_PIXEL_DENSITY);
	checkError((window == nullptr), SDL_GetError());
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	// main renderer
	renderer = SDL_CreateRenderer(window, nullptr);
	checkError((renderer == nullptr), SDL_GetError());
	// main texture, or screen to draw to
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
	checkError((texture == nullptr), SDL_GetError());
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	// main audiostream
	audiostream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
	checkError((audiostream == nullptr), SDL_GetError());
	SDL_ResumeAudioStreamDevice(audiostream);
	// clear SDL events
	SDL_zero(e);

	/* imgui */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// custom font
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	myFont = io.Fonts->AddFontFromFileTTF("res/RobotoMono-Regular.ttf", 20.0f);
	checkError((myFont == nullptr), "ImGui failed to load custom font");
	// handler for saving custom color settings
	//ImGuiSettingsHandler handler;
	//handler.TypeName = "Custom_Settings";
	//handler.TypeHash = ImHashStr("Custom_Settings");
	//handler.ReadOpenFn = Settings_ReadOpen;
	//handler.ReadLineFn = Settings_ReadLine;
	//handler.WriteAllFn = Settings_WriteAll;
	//ImGui::GetCurrentContext()->SettingsHandlers.push_back(handler);
	// platform/renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
}

void GUI::shutdownGui() {
	// safely shutdown ImGui
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	// safely shutdown SDL
	if (audiostream) { SDL_DestroyAudioStream(audiostream); audiostream = nullptr; }
	if (texture) { SDL_DestroyTexture(texture); texture = nullptr; }
	if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
	if (window) { SDL_DestroyWindow(window); window = nullptr; }
	SDL_Quit();
}

void GUI::eventHandler(c8ke& emulator) {
	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL3_ProcessEvent(&e);

		// handle quitting
		if (e.type == SDL_EVENT_QUIT) {
			emulator.state = QUIT;
			return;
		}

		// handle pausing
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_P) {
			emulator.state = (emulator.state == RUNNING) ? PAUSED : RUNNING;
			return;
		}

		// handle all other input
		if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
			auto key = keymap.find(e.key.key);
			if (key != keymap.end()) {
				bool pressed = (e.type == SDL_EVENT_KEY_DOWN);
				emulator.input[key->second] = pressed;
				if (emulator.state == HALT && !pressed) {
					emulator.regs[emulator.temp] = key->second;
					emulator.state = RUNNING;
				}
			}
		}

	}
}

void GUI::beep(bool beep) {
	if (!beep) { SDL_ClearAudioStream(audiostream); return; }

	const int total = SDL_min(customAudio.beepAmount / sizeof(float), 128); // how many float samples to generate (100 bytes worth, capped at 128 samples)
	float samples[128];  // array to hold generated audio samples

	for (int i = 0; i < total; i++) {
		float gain = (i < 32) ? (float)i / 32.0f : 1.0f; // volume gradually increases over the first 32 samples
		float phase = current_sine_sample++ * customAudio.beepPhase / 8000.0f; // calculate the current phase of the sine wave (2200 Hz tone, 8000 Hz sample rate)
		samples[i] = SDL_sinf(phase * 2.0f * SDL_PI_F) * gain; // generate sine wave sample and apply the gain
	}

	current_sine_sample %= 8000; // prevent the sine sample index from growing too large over time
	SDL_PutAudioStreamData(audiostream, samples, total * sizeof(float)); // queue the generated samples to the audio stream
}

void GUI::drawScreen(c8ke& emulator) {
	/* ImGui */
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// top menu bar
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255)); // white
	ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(60, 60, 60, 255)); // gray
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(60, 60, 60, 255)); // gray
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(60, 60, 60, 255)); // gray
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", nullptr)) {
				char const* filterPatterns[1] = { "*.ch8" };
				char* openFileName = tinyfd_openFileDialog("Choose a CHIP-8 rom file to open", nullptr, 1, filterPatterns, "CH8 File", 1);
				if (openFileName) {
					emulator.romPath = openFileName;
					std::replace(emulator.romPath.begin(), emulator.romPath.end(), '\\', '/');
					emulator.state = RELOAD;
				} else if (emulator.romPath.empty()) {
					emulator.state = INIT;
				} else if (emulator.state == HALT) {
					emulator.state = DELAY_HALT;
				} else {
					emulator.state = DELAYED;
				}
			} ImGui::Separator();

			if (ImGui::MenuItem("Reset", nullptr)) {
				if (!emulator.romPath.empty()) emulator.state = RELOAD;
			} ImGui::Separator();

			if (ImGui::MenuItem("Close", nullptr)) {
				emulator.state = RESET;
			} ImGui::Separator();

			if (ImGui::MenuItem("Quit", nullptr)) {
				emulator.state = QUIT;
			}
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings")) {
			if (ImGui::BeginMenu("Colors")) {
				if (ImGui::ColorButton("##emuFg", customColors.emuFg, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showFgPicker = !showFgPicker;
				ImGui::SameLine();
				ImGui::Text("Emulator Fg Color");
				if (showFgPicker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##emuFgPicker", (float*)&customColors.emuFg, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##emuBg", customColors.emuBg, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showBgPicker = !showBgPicker;
				ImGui::SameLine();
				ImGui::Text("Emulator Bg Color");
				if (showBgPicker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##emuBgPicker", (float*)&customColors.emuBg, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##dbgColor1", customColors.dbgColor1, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showDbgColor1Picker = !showDbgColor1Picker;
				ImGui::SameLine();
				ImGui::Text("DBG: Fg Color 1");
				if (showDbgColor1Picker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##dbgColor1Picker", (float*)&customColors.dbgColor1, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##dbgColor2", customColors.dbgColor2, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showDbgColor2Picker = !showDbgColor2Picker;
				ImGui::SameLine();
				ImGui::Text("DBG: Fg Color 2");
				if (showDbgColor2Picker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##dbgColor2Picker", (float*)&customColors.dbgColor2, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##dbgColor3", customColors.dbgColor3, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showDbgColor3Picker = !showDbgColor3Picker;
				ImGui::SameLine();
				ImGui::Text("DBG: Fg Color 3");
				if (showDbgColor3Picker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##dbgColor3Picker", (float*)&customColors.dbgColor3, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##dbgBg", customColors.dbgBg, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showDbgBgPicker = !showDbgBgPicker;
				ImGui::SameLine();
				ImGui::Text("DBG: Bg Color");
				if (showDbgBgPicker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##dbgBgColor", (float*)&customColors.dbgBg, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##dbgHeaderFg", customColors.dbgHeaderFg, COLOR_EDIT_FLAGS_COLOR_BUTTON, ImVec2(20, 20))) showDbgHeaderFgPicker = !showDbgHeaderFgPicker;
				ImGui::SameLine();
				ImGui::Text("DBG: Header Fg Color");
				if (showDbgHeaderFgPicker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##dbgHeaderFgPicker", (float*)&customColors.dbgHeaderFg, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::ColorButton("##dbgHeaderBg", customColors.dbgHeaderBg, COLOR_EDIT_FLAGS_PICKER, ImVec2(20, 20))) showDbgHeaderBgPicker = !showDbgHeaderBgPicker;
				ImGui::SameLine();
				ImGui::Text("DBG: Header Bg Color");
				if (showDbgHeaderBgPicker) {
					ImGui::SameLine(0, 10);
					ImGui::ColorPicker4("##dbgHeaderBgPicker", (float*)&customColors.dbgHeaderBg, COLOR_EDIT_FLAGS_PICKER);
				} ImGui::Separator();

				if (ImGui::MenuItem("Reset to default")) {
					customColors.emuFg = DEFAULT_EMULATOR_FG;
					customColors.emuBg = DEFAULT_EMULATOR_BG;
					customColors.dbgColor1 = DEFAULT_DEBUG_COLOR_1;
					customColors.dbgColor2 = DEFAULT_DEBUG_COLOR_2;
					customColors.dbgColor3 = DEFAULT_DEBUG_COLOR_3;
					customColors.dbgBg = DEFAULT_DEBUG_BG;
					customColors.dbgHeaderFg = DEFAULT_DEBUG_HEADER_FG;
					customColors.dbgHeaderBg = DEFAULT_DEBUG_HEADER_BG;
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Audio")) {
				ImGui::SetNextItemWidth(150);
				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(19, 19, 19, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 60, 60, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(60, 60, 60, 255));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(19, 19, 19, 255));
				ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(60, 60, 60, 255));
				ImGui::InputInt("Beep Adjustment 1", &customAudio.beepAmount);
				ImGui::PopStyleColor(5);
				ImGui::Separator();

				ImGui::SetNextItemWidth(150);
				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(19, 19, 19, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 60, 60, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(60, 60, 60, 255));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(19, 19, 19, 255));
				ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(60, 60, 60, 255));
				ImGui::InputInt("Beep Adjustment 2", &customAudio.beepPhase);
				ImGui::PopStyleColor(5);
				ImGui::Separator();

				if (ImGui::MenuItem("Reset to default")) {
					customAudio.beepAmount = DEFAULT_BEEP_AMOUNT;
					customAudio.beepPhase = DEFAULT_BEEP_PHASE;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		} else {
			showFgPicker = false;
			showBgPicker = false;
			showDbgColor1Picker = false;
			showDbgColor2Picker = false;
			showDbgColor3Picker = false;
			showDbgBgPicker = false;
			showDbgHeaderFgPicker = false;
			showDbgHeaderBgPicker = false;
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleColor(4);

	// emulator screen
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
	ImGui::SetNextWindowSize(ImVec2(ORIGINAL_WIDTH * scale, ORIGINAL_HEIGHT * scale));
	ImGui::Begin("CHIP-8 Screen", nullptr, emulatorScreen);
	ImGui::Image((ImTextureID)texture, ImVec2(texture->w * scale, texture->h * scale));
	ImVec2 chip8_screen_pos = ImGui::GetWindowPos();
	ImVec2 chip8_screen_size = ImGui::GetWindowSize();
	ImGui::End();
	ImGui::PopStyleVar(2);

	// controls
	ImGui::PushStyleColor(ImGuiCol_Text, customColors.dbgHeaderFg);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, customColors.dbgHeaderBg);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColors.dbgBg);
	ImGui::SetNextWindowPos(ImVec2(chip8_screen_pos.x + chip8_screen_size.x, chip8_screen_pos.y));
	ImGui::SetNextWindowSize(ImVec2(windowWidth - chip8_screen_size.x, chip8_screen_size.y / 2 - 15));
	ImGui::Begin("Controls", nullptr, nonscrollable);

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) { // chip8 keys
			uint8_t chip8Key = chip8Keys[row][col];
			ImGui::TextColored(customColors.dbgColor2, "%X ", chip8Key);
			ImGui::SameLine();
		}

		ImGui::TextColored(customColors.dbgColor1, " < = >  ");
		ImGui::SameLine();

		for (int col = 0; col < 4; ++col) { // mapped keys
			uint8_t chip8Key = chip8Keys[row][col];
			SDL_Keycode keycode = findSDLKeycode(chip8Key);
			const char* keyName = (keycode != SDLK_UNKNOWN) ? SDL_GetKeyName(keycode) : "?";
			ImGui::TextColored(customColors.dbgColor2, "%s ", keyName);
			if (col < 3) ImGui::SameLine();
		}
	}

	const char* text = "Pause [P]";
	float a = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text).x;
	ImGui::SetCursorPosX((a - textWidth) * 0.5f);
	ImGui::TextColored(customColors.dbgColor2, text);
	ImGui::End();
	ImGui::PopStyleColor(3);

	// main registers
	ImGui::PushStyleColor(ImGuiCol_Text, customColors.dbgHeaderFg);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, customColors.dbgHeaderBg);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColors.dbgBg);
	ImGui::SetNextWindowPos(ImVec2(chip8_screen_pos.x + chip8_screen_size.x, chip8_screen_pos.y + (chip8_screen_size.y / 2) - 15));
	ImGui::SetNextWindowSize(ImVec2(windowWidth - chip8_screen_size.x, chip8_screen_size.y / 2 + 15));
	ImGui::Begin("Main Registers", nullptr, nonscrollable);
	ImVec4 currentColor;

	ImGui::TextColored(customColors.dbgColor1, "Program Counter");
	ImGui::SameLine();
	currentColor = (emulator.pc == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
	ImGui::TextColored(currentColor, "\t%04X", emulator.pc);

	ImGui::TextColored(customColors.dbgColor1, "Stack Pointer  ");
	ImGui::SameLine();
	currentColor = (emulator.sp == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
	ImGui::TextColored(currentColor, "\t%04X", emulator.sp);

	ImGui::TextColored(customColors.dbgColor1, "Cur Instruction");
	ImGui::SameLine();
	currentColor = (emulator.ins == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
	ImGui::TextColored(currentColor, "\t%04X", emulator.ins);

	ImGui::TextColored(customColors.dbgColor1, "Index Pointer  ");
	ImGui::SameLine();
	currentColor = (emulator.index == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
	ImGui::TextColored(currentColor, "\t%04X", emulator.index);

	ImGui::TextColored(customColors.dbgColor1, "Delay Timer    ");
	ImGui::SameLine();
	currentColor = (emulator.delay == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
	ImGui::TextColored(currentColor, "\t%04X", emulator.delay);

	ImGui::TextColored(customColors.dbgColor1, "Sound Timer    ");
	ImGui::SameLine();
	currentColor = (emulator.sound == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
	ImGui::TextColored(currentColor, "\t%04X", emulator.sound);

	ImGui::End();
	ImGui::PopStyleColor(3);

	// general registers
	ImGui::PushStyleColor(ImGuiCol_Text, customColors.dbgHeaderFg);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, customColors.dbgHeaderBg);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColors.dbgBg);
	ImGui::SetNextWindowPos(ImVec2(chip8_screen_pos.x + chip8_screen_size.x, chip8_screen_pos.y + chip8_screen_size.y));
	ImGui::SetNextWindowSize(ImVec2((windowWidth - chip8_screen_size.x) / 2, windowHeight - chip8_screen_size.y - ImGui::GetFrameHeight()));
	ImGui::Begin("Registers", nullptr, nonscrollable);
	for (int i = 0; i < 16; i++) {
		ImGui::TextColored(customColors.dbgColor1, "0x%01X", i);
		ImGui::SameLine();
		currentColor = (emulator.regs[i] == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
		ImGui::TextColored(currentColor, "%04X", emulator.regs[i]);
	}
	ImGui::End();
	ImGui::PopStyleColor(3);

	// stack
	ImGui::PushStyleColor(ImGuiCol_Text, customColors.dbgHeaderFg);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, customColors.dbgHeaderBg);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColors.dbgBg);
	ImGui::SetNextWindowPos(ImVec2(chip8_screen_pos.x + chip8_screen_size.x + ((windowWidth - chip8_screen_size.x) / 2), chip8_screen_pos.y + chip8_screen_size.y));
	ImGui::SetNextWindowSize(ImVec2((windowWidth - chip8_screen_size.x) / 2, windowHeight - chip8_screen_size.y - ImGui::GetFrameHeight()));
	ImGui::Begin("Stack", nullptr, nonscrollable);
	for (int i = 0; i < 16; i++) {
		ImGui::TextColored(customColors.dbgColor1, "0x%01X", i);
		ImGui::SameLine();
		currentColor = (emulator.stack[i] == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
		ImGui::TextColored(currentColor, "%04X", emulator.stack[i]);
	}
	ImGui::End();
	ImGui::PopStyleColor(3);

	// memory
	ImGui::PushStyleColor(ImGuiCol_Text, customColors.dbgHeaderFg);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, customColors.dbgHeaderBg);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, customColors.dbgHeaderBg);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customColors.dbgBg);
	ImGui::SetNextWindowPos(ImVec2(chip8_screen_pos.x, chip8_screen_pos.y + chip8_screen_size.y));
	ImGui::SetNextWindowSize(ImVec2(chip8_screen_size.x, windowHeight - chip8_screen_size.y - ImGui::GetFrameHeight()));
	ImGui::Begin("Memory", nullptr, scrollable);
	bool byte2 = false;

	for (int i = 0; i < SIZE_MEM; i += 16) {
		ImGui::TextColored(customColors.dbgColor1, "0x%04X\t", i);
		ImGui::SameLine();
		for (int j = 0; j < 16; j++) {
			unsigned char byte = emulator.mem[i + j];
			currentColor = (byte == 0) ? customColors.dbgColor3 : customColors.dbgColor2;
			ImGui::TextColored(currentColor, "%02X", byte);
			ImGui::SameLine();
			if (byte2 && j < 15) {
				ImGui::Text(" ");
				ImGui::SameLine();
			}
			byte2 = !byte2;
		}
		ImGui::NewLine();
	}

	ImGui::End();
	ImGui::PopStyleColor(4);

	/* SDL */
	Uint8 fr = (Uint8)(customColors.emuFg.x * 255.0f);
	Uint8 fg = (Uint8)(customColors.emuFg.y * 255.0f);
	Uint8 fb = (Uint8)(customColors.emuFg.z * 255.0f);
	Uint8 fa = (Uint8)(customColors.emuFg.w * 255.0f);
	Uint8 br = (Uint8)(customColors.emuBg.x * 255.0f);
	Uint8 bg = (Uint8)(customColors.emuBg.y * 255.0f);
	Uint8 bb = (Uint8)(customColors.emuBg.z * 255.0f);
	Uint8 ba = (Uint8)(customColors.emuBg.w * 255.0f);

	// draw the emulator screen
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, br, bg, bb, ba);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, fr, fg, fb, fa);
	for (int y = 0; y < ORIGINAL_HEIGHT; y++) {
		for (int x = 0; x < ORIGINAL_WIDTH; x++) {
			if (emulator.screen[y][x]) {
				SDL_FRect pixel = { (float)x, (float)y, 1, 1 }; // render 1x1 pixels
				SDL_RenderFillRect(renderer, &pixel);
			}
		}
	}
	SDL_SetRenderTarget(renderer, nullptr);

	/* render and present everything */
	ImGui::Render();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}
