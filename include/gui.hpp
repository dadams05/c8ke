#pragma once
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "globals.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "SDL3/SDL.h"
#include "tinyfiledialogs.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>


class c8ke; // forward declaration

/* for embedding custom font */
extern unsigned char RobotoMono_Regular_ttf[];
extern unsigned int RobotoMono_Regular_ttf_len;

/* for drawing debug controls */
constexpr uint8_t chip8Keys[4][4] = {
	{ 0x1, 0x2, 0x3, 0xC },
	{ 0x4, 0x5, 0x6, 0xD },
	{ 0x7, 0x8, 0x9, 0xE },
	{ 0xA, 0x0, 0xB, 0xF },
};

/* for translating/remapping keystrokes */
const std::unordered_map<SDL_Keycode, uint8_t> keymap = {
	{SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
	{SDLK_Q, 0x4}, {SDLK_W, 0x5}, {SDLK_E, 0x6}, {SDLK_R, 0xD},
	{SDLK_A, 0x7}, {SDLK_S, 0x8}, {SDLK_D, 0x9}, {SDLK_F, 0xE},
	{SDLK_Z, 0xA}, {SDLK_X, 0x0}, {SDLK_C, 0xB}, {SDLK_V, 0xF},
};

/* ImGui constants */
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

/* for saving custom color and audio settings */
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
struct CustomAudio {
	int beepAmount = DEFAULT_BEEP_AMOUNT;
	int beepPhase = DEFAULT_BEEP_PHASE;
};
static void* Settings_ReadOpen(ImGuiContext*, ImGuiSettingsHandler* handler, const char* name);
static void Settings_ReadLine(ImGuiContext*, ImGuiSettingsHandler* handler, void* user_data, const char* line);
static void Settings_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf);

/* main GUI class */
class GUI {
private:
	/* SDL */
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_AudioStream* audiostream = nullptr;
	SDL_AudioSpec spec = { SDL_AUDIO_F32, 1, 8000 }; // format, channels, frequency
	SDL_Event e{};
	/* ImGui */
	ImGuiSettingsHandler handler;
	ImFontConfig config;
	ImFont* myFont = nullptr;
	/* vars */
	int windowWidth = 1000; // width of the entire window
	int windowHeight = 800; // height of the entire window
	float scale = 11.0f; // scale emulator screen for modern monitors
	bool showFgPicker{}; // show foreground color picker
	bool showBgPicker{}; // show background color picker
	bool showDbgColor1Picker{}; // show debug color picker 1
	bool showDbgColor2Picker{}; // show debug color picker 2
	bool showDbgColor3Picker{}; // show debug color picker 3
	bool showDbgBgPicker{}; // show debug background color picker
	bool showDbgHeaderFgPicker{}; // show debug header foreground color picker
	bool showDbgHeaderBgPicker{}; // show debug header background color picker
public:
	/* main GUI functions */
	void initializeGui(); // initializes SDL and ImGui subsystems
	void shutdownGui(); // safely shuts down the SDL and ImGui subsystems
	void eventHandler(c8ke& emulator); // handles all user input
	void beep(bool beep); // makes noise based on the sound register
	void drawScreen(c8ke& emulator); // draws the window and the program
	/* misc */
	SDL_Keycode findSDLKeycode(uint8_t chip8Key); // translates keystrokes
	int current_sine_sample = 0; // something for the beep
	CustomAudio customAudio;
	CustomColors customColors;
};
