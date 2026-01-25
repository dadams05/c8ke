#pragma once
#include <stdint.h>

constexpr uint8_t ORIGINAL_WIDTH = 64; // original screen width in pixels
constexpr uint8_t ORIGINAL_HEIGHT = 32; // original screen height in pixels

constexpr int DEFAULT_BEEP_AMOUNT = 100; // beep parameter 1
constexpr int DEFAULT_BEEP_PHASE = 2200; // beep parameter 2

constexpr uint16_t SIZE_MEM = 0x1000; // 4kB memory, 4096 bytes

// state machine for the emulator
enum State {
	INIT = 0,
	RUNNING,
	QUIT,
	PAUSED,
	RELOAD,
	DELAYED,
	HALT,
	DELAY_HALT,
	RESET,
};