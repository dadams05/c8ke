#include "SDL3/SDL_main.h"
#include "c8ke.hpp"
#include "gui.hpp"

c8ke emulator;
GUI gui;

void run() {
	long long elapsed{};
	double cycleDelta{}, refreshDelta{};
	std::chrono::high_resolution_clock::time_point now{}, last{};

	last = std::chrono::high_resolution_clock::now();

	// main loop
	while (emulator.state != QUIT) {
		// reset loaded rom
		if (emulator.state == RELOAD) {
			emulator.clearScreen();
			emulator.resetEmulator();
			emulator.loadRomFile(emulator.romPath);
			emulator.state = RUNNING;
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();
		}
		// reset timing so emulator does not over-compensate
		if (emulator.state == DELAYED) {
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();
			emulator.state = RUNNING;
		}
		// completely reset the emulator, except for custom colors
		if (emulator.state == RESET) {
			//clearScreen();
			emulator.resetEmulator();
			emulator.romPath = "";

			emulator.state = INIT;
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();

			emulator.ins = 0;
		}
		// setup for CHIP-8 halt instruction
		if (emulator.state == DELAY_HALT) {
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();
			emulator.state = HALT;
		}
		// calculate new timings
		now = std::chrono::high_resolution_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last).count();
		cycleDelta += elapsed;
		refreshDelta += elapsed;

		// handles events, input
		gui.eventHandler(emulator.state, emulator.input, emulator.regs, emulator.temp);

		// cycle instructions
		while (cycleDelta >= CYCLE_TIME) {
			cycleDelta -= CYCLE_TIME;
			if (emulator.state == RUNNING) emulator.cycleEmulator();
		}
		// update screen, sound, delay
		if (refreshDelta >= REFRESH_TIME) {
			refreshDelta -= REFRESH_TIME;
			gui.drawScreen(emulator.state, emulator.romPath, emulator.screen);

			if (emulator.delay > 0) emulator.delay--;
			if (emulator.sound > 0) emulator.sound--;
		}

		// actual sound
		gui.beep(emulator.sound > 0);

		// update for next cycle
		last = now;
	}
}

int main(int argc, char* args[]) {
	emulator.resetEmulator();
	gui.initializeGui();
	run();
	gui.shutdownGui();
	return EXIT_SUCCESS;
}
