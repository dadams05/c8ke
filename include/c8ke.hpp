#pragma once
#include "consts.hpp"
#include <ios>
#include <chrono>
#include <random>
#include <string>
#include <fstream>
#include <iostream>

using std::string;

// memory

constexpr uint16_t DEF_ADDR = 0x200; // default address; start address of user program
constexpr uint8_t SPRITE_ADDR = 0x50; // start address of sprite data
constexpr uint8_t SPRITE_SIZE = 80; // sprite data memory size in bytes
constexpr uint8_t SPRITES[SPRITE_SIZE] = { // sprites to store in memory
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
			0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

// timing
constexpr uint16_t CLK = 500; // 500 Hz; 500 cycles/sec
constexpr double CYCLE_TIME = 1000000000.0 / CLK;
constexpr uint8_t FPS = 60; // 60 FPS; 60 frames/sec
constexpr double REFRESH_TIME = 1000000000.0 / FPS;

// main emulator class
class c8ke {
public:
	// emulator registers
	uint16_t pc{}; // program counter
	uint8_t sp{}; // stack pointer
	uint16_t stack[16]{}; // stack
	uint8_t regs[16]{}; // general purpose registers
	uint8_t delay{}; // delay register
	uint8_t sound{}; // sound register
	uint16_t index{}; // index register
	// important vars
	uint8_t screen[ORIGINAL_HEIGHT][ORIGINAL_WIDTH] = { 0 };
	uint8_t mem[SIZE_MEM]{}; // emulator memory
	uint16_t ins{}; // current instruction
	State state{}; // current state of the emulator
	string romPath{}; // the path of the loaded ROM file
	bool input[16]{}; // has the states of all the keys
	// other vars
	uint8_t temp{}; // temporary value holder
	int currentSine{}; // something for the beep
	
	// methods
	void resetEmulator(void); // reset the emulator to a fresh state
	void clearScreen(void); // reset the screen array to clear the screen
	void loadRomFile(std::string path); // load a ROM file into the emulator
	void cycleEmulator(void); // fetch, decode, and execute an instruction
};
