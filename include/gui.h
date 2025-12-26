#pragma once //#include "SDL3_image/SDL_image.h" // v3.2.4

#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "tinyfiledialogs.h"

#include <cstring>
#include <unordered_map>

// Emulator display
constexpr uint8_t ORIGINAL_WIDTH = 64; // original interpreter screen width
constexpr uint8_t ORIGINAL_HEIGHT = 32; // original interpreter screen height

// globals
extern float scale; // scale emulator screen for modern monitors
extern uint8_t screen[ORIGINAL_HEIGHT][ORIGINAL_WIDTH]; // original interpreter screen
extern const char* name; // window name
extern int width; // actual window width
extern int height; // actual window height

extern std::unordered_map<SDL_Keycode, uint8_t> keymap; // input keys remapped

// SDL
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* texture;
extern SDL_AudioStream* audiostream;
extern SDL_AudioSpec spec; // = { SDL_AUDIO_F32, 1, 8000 }; // format, channels, frequency
extern SDL_Event e;

// ImGui
extern ImFont* myFont;
inline constexpr ImGuiColorEditFlags COLOR_EDIT_FLAGS_COLOR_BUTTON = ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreviewHalf;
inline constexpr ImGuiColorEditFlags COLOR_EDIT_FLAGS_PICKER = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview;
inline constexpr ImGuiWindowFlags nonscrollable = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus;
inline constexpr ImGuiWindowFlags scrollable = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
inline constexpr ImGuiWindowFlags emulatorScreen = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
extern bool showFgPicker;
extern bool showBgPicker;
extern bool showDbgColor1Picker;
extern bool showDbgColor2Picker;
extern bool showDbgColor3Picker;
extern bool showDbgBgPicker;
extern bool showDbgHeaderFgPicker;
extern bool showDbgHeaderBgPicker;
inline constexpr ImVec4 DEFAULT_EMULATOR_FG = ImVec4(255.0f / 255.0f, 161.0f / 255.0f, 105.0f / 255.0f, 1.0f); // pastel orange
inline constexpr ImVec4 DEFAULT_EMULATOR_BG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black
inline constexpr ImVec4 DEFAULT_DEBUG_COLOR_1 = ImVec4(255.0f / 255.0f, 161.0f / 255.0f, 105.0f / 255.0f, 1.0f); // pastel orange
inline constexpr ImVec4 DEFAULT_DEBUG_COLOR_2 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white
inline constexpr ImVec4 DEFAULT_DEBUG_COLOR_3 = ImVec4(75.0f / 255.0f, 75.0f / 255.0f, 75.0f / 255.0f, 1.0f); // gray
inline constexpr ImVec4 DEFAULT_DEBUG_BG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black
inline constexpr ImVec4 DEFAULT_DEBUG_HEADER_FG = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white
inline constexpr ImVec4 DEFAULT_DEBUG_HEADER_BG = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black
struct CustomColors {
	ImVec4 emuFg;
	ImVec4 emuBg;
	ImVec4 dbgColor1;
	ImVec4 dbgColor2;
	ImVec4 dbgColor3;
	ImVec4 dbgBg;
	ImVec4 dbgHeaderFg;
	ImVec4 dbgHeaderBg;
};
extern CustomColors customColors;

// main functions
void initializeGui(void);
void shutdownGui(void);
void clearScreen(void);
void eventHandler(void);
void drawScreen(void);
void beep(bool beep);
SDL_Keycode findSDLKeycode(uint8_t chip8Key);
static void Settings_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf);
static void Settings_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* user_data, const char* line);
static void* Settings_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);
