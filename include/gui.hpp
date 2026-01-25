//#include "SDL3_image/SDL_image.h" // v3.2.4
#pragma once
#include "consts.hpp"
#include "SDL3/SDL.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "tinyfiledialogs.h"
#include <string>
#include <iostream>
#include <cstring>
#include <unordered_map>
#include <algorithm>


// for drawing debug controls
constexpr uint8_t chip8Keys[4][4] = {
	{ 0x1, 0x2, 0x3, 0xC },
	{ 0x4, 0x5, 0x6, 0xD },
	{ 0x7, 0x8, 0x9, 0xE },
	{ 0xA, 0x0, 0xB, 0xF },
};

// for translating/remapping keystrokes
const std::unordered_map<SDL_Keycode, uint8_t> keymap = {
	{SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
	{SDLK_Q, 0x4}, {SDLK_W, 0x5}, {SDLK_E, 0x6}, {SDLK_R, 0xD},
	{SDLK_A, 0x7}, {SDLK_S, 0x8}, {SDLK_D, 0x9}, {SDLK_F, 0xE},
	{SDLK_Z, 0xA}, {SDLK_X, 0x0}, {SDLK_C, 0xB}, {SDLK_V, 0xF},
};




struct CustomAudio {
	int beepAmount{};
	int beepPhase{};
};


// ImGui constants
constexpr ImGuiColorEditFlags COLOR_EDIT_FLAGS_COLOR_BUTTON = ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreviewHalf;
constexpr ImGuiColorEditFlags COLOR_EDIT_FLAGS_PICKER = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview;
constexpr ImGuiWindowFlags nonscrollable = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus;
constexpr ImGuiWindowFlags scrollable = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
constexpr ImGuiWindowFlags emulatorScreen = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
constexpr ImVec4 DEFAULT_EMULATOR_FG = ImVec4(255.0f / 255.0f, 161.0f / 255.0f, 105.0f / 255.0f, 1.0f); // pastel orange
constexpr ImVec4 DEFAULT_EMULATOR_BG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black
constexpr ImVec4 DEFAULT_DEBUG_COLOR_1 = ImVec4(255.0f / 255.0f, 161.0f / 255.0f, 105.0f / 255.0f, 1.0f); // pastel orange
constexpr ImVec4 DEFAULT_DEBUG_COLOR_2 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white
constexpr ImVec4 DEFAULT_DEBUG_COLOR_3 = ImVec4(75.0f / 255.0f, 75.0f / 255.0f, 75.0f / 255.0f, 1.0f); // gray
constexpr ImVec4 DEFAULT_DEBUG_BG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black
constexpr ImVec4 DEFAULT_DEBUG_HEADER_FG = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white
constexpr ImVec4 DEFAULT_DEBUG_HEADER_BG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black

//static void Settings_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf, struct );
//static void Settings_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* user_data, const char* line);
//static void* Settings_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

// for saving custom color setup data
struct CustomColors {
	ImVec4 emuFg = DEFAULT_EMULATOR_FG;
	ImVec4 emuBg = DEFAULT_EMULATOR_BG;
	ImVec4 dbgColor1 = DEFAULT_DEBUG_COLOR_1;
	ImVec4 dbgColor2 = DEFAULT_DEBUG_COLOR_2;
	ImVec4 dbgColor3 = DEFAULT_DEBUG_COLOR_3;
	ImVec4 dbgBg = DEFAULT_DEBUG_BG;
	ImVec4 dbgHeaderFg = DEFAULT_DEBUG_HEADER_FG;
	ImVec4 dbgHeaderBg = DEFAULT_DEBUG_HEADER_BG;
};



// main gui class
class GUI {
private:
	// SDL
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_AudioStream* audiostream = nullptr;
	SDL_AudioSpec spec = { SDL_AUDIO_F32, 1, 8000 }; // format, channels, frequency
	SDL_Event e{};
	// ImGui
	ImFont* myFont = nullptr;
	// vars
	int windowWidth = 1000; // width of the entire window
	int windowHeight = 800; // height of the entire window
	float scale = 11.0f; // scale emulator screen for modern monitors
	bool showFgPicker{};
	bool showBgPicker{};
	bool showDbgColor1Picker{};
	bool showDbgColor2Picker{};
	bool showDbgColor3Picker{};
	bool showDbgBgPicker{};
	bool showDbgHeaderFgPicker{};
	bool showDbgHeaderBgPicker{};
	
public:
	// main gui functions
	void initializeGui(void);
	void shutdownGui(void);
	void eventHandler(bool state, bool input[], uint8_t regs[], uint8_t temp);
	void drawScreen(bool state, std::string romPath, uint8_t screen[ORIGINAL_HEIGHT][ORIGINAL_WIDTH]);
	void beep(bool beep);
	// utility functions
	SDL_Keycode findSDLKeycode(uint8_t chip8Key);
	int current_sine_sample = 0; // something for the beep
	CustomAudio customAudio;
	CustomColors customColors;
};










