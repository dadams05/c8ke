#pragma once

#include <ios>
#include <chrono>
#include <random>
#include <string>
#include <fstream>
#include <iostream>

// state machine for the emulator
enum State {
	INIT,
	RUNNING,
	QUIT,
	PAUSED,
	RELOAD,
	DELAYED,
	HALT,
	DELAY_HALT,
	RESET,
};

// consts
constexpr uint16_t SIZE_MEM = 0x1000; // 4kB memory, 4096 bytes
constexpr uint16_t DEF_ADDR = 0x200; // default address; start address of user program
constexpr uint8_t SPRITE_ADDR = 0x50; // start address of sprite data
constexpr uint8_t SPRITE_SIZE = 80; // sprite data memory size in bytes
inline constexpr uint8_t SPRITES[SPRITE_SIZE] = { // sprites to store in memory
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80, // D
};

// timing consts
constexpr uint8_t CLK = 500; // 500 Hz, 500 cycles/sec
constexpr uint8_t FPS = 60; // 60 FPS, 60 frames/sec
constexpr double CYCLE_TIME = 1000000000.0 / CLK;
constexpr double REFRESH_TIME = 1000000000.0 / FPS;

// globals
extern std::string romPath; // the path to the loaded ROM file
extern uint8_t tempReg; // needed for one input instruction
extern bool input[16]; // has pressed keys
extern int current_sine_sample; // something for the beep
constexpr uint8_t chip8Keys[4][4] = { // for drawing debug controls
	{0x1, 0x2, 0x3, 0xC},
	{0x4, 0x5, 0x6, 0xD},
	{0x7, 0x8, 0x9, 0xE},
	{0xA, 0x0, 0xB, 0xF},
};

// audio
constexpr int DEFAULT_BEEP_AMOUNT = 100; // beep parameter 1
constexpr int DEFAULT_BEEP_PHASE = 2200; // beep parameter 2
struct CustomAudio {
	int beepAmount;
	int beepPhase;
};
extern CustomAudio customAudio;

// helper functions
void checkError(bool cond, std::string msg); // for checking conditions and exiting if failing

// main emulator class
class c8ke {
public:
	uint16_t pc{}; // program counter
	uint16_t stack[16]{}; // stack
	uint8_t sp{}; // stack pointer
	uint8_t regs[16]{}; // general purpose registers
	uint16_t index{}; // index register
	uint8_t delay{}; // delay register
	uint8_t sound{}; // sound register
	uint16_t ins{}; // current instruction
	uint8_t mem[SIZE_MEM]{}; // emulator memory
	
	void resetEmulator(void); // reset the emulator to a fresh state
	void loadRomFile(std::string path); // load a ROM file into the emulator
	void cycleEmulator(void); // fetch, decode, and execute an instruction
	void runEmulator(void); // main run loop
};

// main emulator object
extern c8ke emulator;
extern State c8keState;
