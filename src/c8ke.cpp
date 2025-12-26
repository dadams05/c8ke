#include "c8ke.h"
#include "gui.h"

#include "SDL3/SDL_main.h"

// define variables
std::string romPath = "";
uint8_t tempReg = 0;
int current_sine_sample = 0;
bool input[16] = { 0 };
CustomAudio customAudio{ DEFAULT_BEEP_AMOUNT, DEFAULT_BEEP_PHASE };
State c8keState = INIT;
c8ke emulator;


void checkError(bool cond, std::string msg) {
	if (cond) {
		std::cerr << "[c8ke] " << msg << std::endl;
		exit(EXIT_FAILURE);
	}
}

void c8ke::resetEmulator(void) {
	// reset registers to default state
	pc = DEF_ADDR;
	std::memset(stack, 0, sizeof(stack));
	sp = -1;
	std::memset(regs, 0, sizeof(regs));
	index = 0;
	delay = 0;
	sound = 0;
	std::memset(mem, 0, SIZE_MEM);
	// load sprites into memory
	for (int i = 0; i < SPRITE_SIZE; i++) {
		mem[SPRITE_ADDR + i] = SPRITES[i];
	}
	// signal in init state
	c8keState = INIT;
}

void c8ke::loadRomFile(std::string path) {
	// open ROM file in binary mode
	std::ifstream rom(path, std::ios::binary);
	checkError(!rom.is_open(), "Failed to open ROM file");
	// read the file byte by byte
	uint8_t byte;
	while (rom.read(reinterpret_cast<char*>(&byte), sizeof(byte))) { mem[pc++] = byte; }
	// reset used variables
	pc = DEF_ADDR;
	rom.close();
	c8keState = RUNNING;
}

void c8ke::cycleEmulator(void) {
	ins = (mem[pc] << 8) | mem[pc + 1];
	pc += 2;

	switch (ins & 0xF000) { // checks the first nibble
	case 0x0000: { // 00E*
		switch (ins & 0x000F) {
		case 0x0: // 00E0: clear the display
			clearScreen();
			break;
		case 0xE: // 00EE: return from a subroutine
			pc = stack[sp];
			sp--;
			break;
		}
	} break;

	case 0x1000: { // 1nnn: jump to location nnn
		pc = ins & 0x0FFF;
	} break;

	case 0x2000: { // 2nnn: call subroutine at nnn
		sp++;
		stack[sp] = pc;
		pc = ins & 0x0FFF;
	} break;

	case 0x3000: { // 3xkk: skip next instruction if Vx = kk
		if (regs[(ins & 0x0F00) >> 8] == (ins & 0x00FF)) pc += 2;
	} break;

	case 0x4000: { // 4xkk: skip next instruction if Vx != kk
		if (regs[(ins & 0x0F00) >> 8] != (ins & 0x00FF)) pc += 2;
	} break;

	case 0x5000: { // 5xy0: skip next instruction if Vx = Vy
		if (regs[(ins & 0x0F00) >> 8] == regs[(ins & 0x00F0) >> 4]) pc += 2;
	} break;

	case 0x6000: { // 6xkk: set Vx = kk
		regs[(ins & 0x0F00) >> 8] = (ins & 0x00FF);
	} break;

	case 0x7000: { // 7xkk: set Vx = Vx + kk
		regs[(ins & 0x0F00) >> 8] += (ins & 0x00FF);
	} break;

	case 0x8000: { // 8xy*
		uint8_t x = (ins & 0x0F00) >> 8;
		uint8_t y = (ins & 0x00F0) >> 4;
		switch (ins & 0x000F) {
		case 0x0: // 8xy0: set Vx = Vy
			regs[x] = regs[y];
			break;
		case 0x1: // 8xy1: set Vx = Vx OR Vy
			regs[x] |= regs[y];
			regs[0xF] = 0;
			break;
		case 0x2: // 8xy2: set Vx = Vx AND Vy
			regs[x] &= regs[y];
			regs[0xF] = 0;
			break;
		case 0x3: // 8xy3: set Vx = Vx XOR Vy
			regs[x] ^= regs[y];
			regs[0xF] = 0;
			break;
		case 0x4: { // 8xy4: set Vx = Vx + Vy, set VF = carry
			uint16_t sum = regs[x] + regs[y];
			regs[x] = sum & 0xFF;
			regs[0xF] = (sum > 0xFF) ? 1 : 0;
		} break;
		case 0x5: {// 8xy5: set Vx = Vx - Vy, set VF = NOT borrow
			uint8_t originalX = regs[x];
			regs[x] -= regs[y];
			regs[0xF] = (originalX >= regs[y]) ? 1 : 0;
		} break;
		case 0x6: {// 8xy6: set Vx = Vx SHR 1
			uint8_t lsb = regs[y] & 0x1;
			regs[x] = regs[y];
			regs[x] >>= 1;
			regs[0xF] = lsb;
		} break;
		case 0x7: { // 8xy7: set Vx = Vy - Vx, set VF = NOT borrow
			uint8_t originalX = regs[x];
			regs[x] = regs[y] - regs[x];
			regs[0xF] = (regs[y] >= originalX) ? 1 : 0;
		} break;
		case 0xE: { // 8xyE: set Vx = Vx SHL 1
			uint8_t msb = (regs[y] & 0x80) >> 7;
			regs[x] = regs[y];
			regs[x] <<= 1;
			regs[0xF] = msb;
		} break;
		}
	} break;

	case 0x9000: { // 9xy0: skip next insruction if Vx != Vy
		if ((regs[(ins & 0x0F00) >> 8]) != (regs[(ins & 0x00F0) >> 4])) pc += 2;
	} break;

	case 0xA000: { // Annn: set i = nnn
		index = ins & 0x0FFF;
	} break;

	case 0xB000: { // Bnnn: jump to location nnn + V0
		pc = (ins & 0x0FFF) + regs[0];
	} break;

	case 0xC000: { // Cxkk: set Vx = random byte AND kk
		std::mt19937 rng(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<int> dist(0, 255);
		regs[(ins & 0x0F00) >> 8] = dist(rng) & (ins & 0x00FF);
	} break;

	case 0xD000: { // Dxyn: display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
		uint8_t x = regs[(ins & 0x0F00) >> 8];
		uint8_t y = regs[(ins & 0x00F0) >> 4];
		uint8_t n = ins & 0x000F;
		regs[0xF] = 0;

		for (int row = 0; row < n; row++) {
			if ((y % ORIGINAL_HEIGHT) + row >= ORIGINAL_HEIGHT) break;
			uint8_t spriteByte = mem[index + row];
			for (int col = 0; col < 8; col++) {
				if ((x % ORIGINAL_WIDTH) + col >= ORIGINAL_WIDTH) break;
				uint8_t pixel = (spriteByte >> (7 - col)) & 0x1;
				uint8_t screenX = (x % ORIGINAL_WIDTH) + col;
				uint8_t screenY = (y % ORIGINAL_HEIGHT) + row;
				if (pixel == 1) {
					if (screen[screenY][screenX] == 1) regs[0xF] = 1;
					screen[screenY][screenX] ^= 1;
				}
			}
		}

	} break;

	case 0xE000: { // Ex**
		uint8_t x = (ins & 0x0F00) >> 8;
		switch (ins & 0x00FF) {
		case 0x9E: // Ex9E: skip next instruction if key with the value of Vx is pressed
			if (input[regs[x]]) pc += 2;
			break;
		case 0xA1: // ExA1: skip next instruction if key with the value of Vx is not pressed
			if (!input[regs[x]]) pc += 2;
			break;
		}
	} break;

	case 0xF000: { // Fx**
		uint8_t x = (ins & 0x0F00) >> 8;
		switch (ins & 0x00FF) {
		case 0x07: // Fx07: set Vx = delay timer value
			regs[x] = delay;
			break;
		case 0x0A:  // Fx0A: wait for a key press, store the value of the key in Vx
			tempReg = x;
			c8keState = HALT;
			break;
		case 0x15: // Fx15: set delay timer = Vx
			delay = regs[x];
			break;
		case 0x18: // Fx18: set sound timer = Vx
			sound = regs[x];
			break;
		case 0x1E: // Fx1E: set i = i + Vx
			index += regs[x];
			break;
		case 0x29: // Fx29: set i = location of sprite for digit Vx
			index = SPRITE_ADDR + (regs[(ins & 0x0F00) >> 8] * 5);
			break;
		case 0x33: { // Fx33: store BCD representation of Vx in memory locations i, i+1, and i+2
			uint8_t number = regs[(ins & 0x0F00) >> 8];
			mem[index] = number / 100;
			mem[index + 1] = (number / 10) % 10;
			mem[index + 2] = number % 10;
		} break;
		case 0x55: { // Fx55: store registers V0 through Vx in memory starting at location i
			uint8_t x = (ins & 0x0F00) >> 8;
			for (int i = 0; i <= x; i++) { mem[index] = regs[i]; index++; }
		} break;
		case 0x65: { // Fx65: read registers V0 through Vx from memory starting at location i
			uint8_t x = (ins & 0x0F00) >> 8;
			for (int i = 0; i <= x; i++) { regs[i] = mem[index]; index++; }
		} break;
		}
	} break;
	}
}

void c8ke::runEmulator(void) {
	double cycleDelta = 0.0, refreshDelta = 0.0;
	long long elapsed = 0;
	std::chrono::high_resolution_clock::time_point now, last;

	last = std::chrono::high_resolution_clock::now();
	while (c8keState != QUIT) { // main loop

		// reset loaded rom
		if (c8keState == RELOAD) {
			clearScreen();
			resetEmulator();
			loadRomFile(romPath);

			c8keState = RUNNING;
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();
		}

		// reset timing so emulator does not over-compensate
		if (c8keState == DELAYED) {
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();
			c8keState = RUNNING;
		}

		// completely reset the emulator, except for custom colors
		if (c8keState == RESET) {
			clearScreen();
			resetEmulator();
			romPath = "";

			c8keState = INIT;
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();

			ins = 0;
		}

		// setup for CHIP-8 halt instruction
		if (c8keState == DELAY_HALT) {
			cycleDelta = 0.0;
			refreshDelta = 0.0;
			last = std::chrono::high_resolution_clock::now();
			c8keState = HALT;
		}

		// calculate new timings
		now = std::chrono::high_resolution_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last).count();
		cycleDelta += elapsed;
		refreshDelta += elapsed;

		// handles events, input
		eventHandler();

		// cycle instructions
		while (cycleDelta >= CYCLE_TIME) {
			cycleDelta -= CYCLE_TIME;
			if (c8keState == RUNNING) cycleEmulator();
		}

		// update screen, sound, delay
		if (refreshDelta >= REFRESH_TIME) {
			refreshDelta -= REFRESH_TIME;
			drawScreen();

			if (delay > 0) delay--;
			if (sound > 0) sound--;
		}

		// actual sound
		beep(sound > 0);

		// update for next cycle
		last = now;
	}

}

int main(int argc, char* args[]) {
	emulator.resetEmulator();
	initializeGui();
	emulator.runEmulator();
	shutdownGui();

	return EXIT_SUCCESS;
}
