#include "SDL3/SDL_main.h"
#include "c8ke.hpp"
#include "gui.hpp"
#include <thread>


c8ke emulator;
GUI gui;

void run() {
	using namespace std::chrono;
	double cycleDelta = 0.0, refreshDelta = 0.0;
	high_resolution_clock::time_point last = high_resolution_clock::now();

	// main loop
	while (emulator.state != QUIT) {
		switch (emulator.state) {
			case RELOAD: // resets the loaded rom
				cycleDelta = refreshDelta = 0.0;
				emulator.resetEmulator();
				emulator.loadRomFile(emulator.romPath);
				emulator.state = RUNNING;
				last = high_resolution_clock::now();
				break;
			case DELAYED: // resets timing so emulator does not overcompensate
				cycleDelta = refreshDelta = 0.0;
				emulator.state = RUNNING;
				last = high_resolution_clock::now();
				break;
			case RESET: // completely resets the emulator, except for custom colors/audio
				cycleDelta = refreshDelta = 0.0;
				emulator.resetEmulator();
				emulator.romPath = "";
				last = high_resolution_clock::now();
				break;
			case DELAY_HALT: // sets up for CHIP-8 halt instruction
				cycleDelta = refreshDelta = 0.0;
				emulator.state = HALT;
				last = high_resolution_clock::now();
				break;
			default:
				break;
		}

		// calculate elapsed time
		high_resolution_clock::time_point now = high_resolution_clock::now();
		auto elapsedNs = duration_cast<nanoseconds>(now - last).count();
		cycleDelta += elapsedNs;
		refreshDelta += elapsedNs;

		// handles input/events
		gui.eventHandler(emulator);

		// run emulator cycles (limit backlog to avoid spiral)
		uint8_t maxCycles = 100;
		while (cycleDelta >= CYCLE_TIME && maxCycles-- > 0) {
			cycleDelta -= CYCLE_TIME;
			if (emulator.state == RUNNING)
				emulator.cycleEmulator();
		}

		// update screen and timers at refresh rate
		if (refreshDelta >= REFRESH_TIME) {
			refreshDelta -= REFRESH_TIME;
			gui.drawScreen(emulator);

			if (emulator.delay > 0) emulator.delay--;
			if (emulator.sound > 0) emulator.sound--;
		}

		// actual sound
		gui.beep(emulator.sound > 0);

		// sleep a tiny amount to reduce CPU usage when idle
		auto sleepNs = std::min(CYCLE_TIME - cycleDelta, REFRESH_TIME - refreshDelta);
		if (sleepNs > 0)
			std::this_thread::sleep_for(nanoseconds(static_cast<long long>(sleepNs)));

		// update timestamp
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
