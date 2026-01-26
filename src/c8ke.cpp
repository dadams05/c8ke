#include "c8ke.hpp"


void c8ke::clearScreen() {
	std::memset(screen, 0, sizeof(screen));
}

void c8ke::resetEmulator() {
	pc = DEF_ADDR;
	sp = -1;
	delay = sound = index = ins = temp = 0;
	state = INIT;
	std::memset(stack, 0, sizeof(stack));
	std::memset(regs, 0, sizeof(regs));
	std::memset(input, 0, sizeof(input));
	std::memset(mem, 0, SIZE_MEM);
	std::memcpy(mem + SPRITE_SIZE, SPRITES, SPRITE_SIZE);
	clearScreen();
}

void c8ke::loadRomFile(const std::string& path) {
	// open ROM file in binary mode
	std::ifstream rom(path, std::ios::binary | std::ios::ate);
	if (!rom.is_open()) exit(EXIT_FAILURE);
	// read the file
	const std::streamsize size = rom.tellg();
	rom.seekg(0);
	rom.read(reinterpret_cast<char*>(&mem[DEF_ADDR]), size);
	state = RUNNING;
}

void c8ke::cycleEmulator() {
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
					temp = x;
					state = HALT;
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
