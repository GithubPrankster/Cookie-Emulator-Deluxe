#include <cstdint>
#include <fstream>
#include <random>
#include <vector>

bool killSwitch = false;

uint16_t operation;
uint16_t prgCounter, indexRegister;

uint8_t mem[4096];
uint8_t registers[16];

std::vector<uint8_t> screen;
uint8_t delayTime, soundTime;

uint16_t stack[16];
uint16_t stackPointer;

uint8_t keys[16];

std::mt19937 rnd;

bool drawn;

void initEmulator(){
	prgCounter = 512;
	operation = 0;
	indexRegister = 0;
	stackPointer = 0;

	for (int i = 0; i < 16; i++) {
		registers[i] = 0;
		stack[i] = 0;
		keys[i] = 0;
	}

	screen.resize(64 * 32);

	for (int r = 0; r < 4096; r++) {
		mem[r] = 0;
	}

	constexpr uint8_t chip8_fontset[80] =
	{
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
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};


	for (int q = 0; q < 80; q++) {
		mem[q] = chip8_fontset[q];
	}

	delayTime = 0;
	soundTime = 0;
	drawn = false;

	rnd = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
}

#include "emuFunctions.h"

void load(std::string filename, unsigned pos = 0x200) {
	for (std::ifstream f(filename, std::ios::binary); f.good(); )
		mem[pos++ & 0xFFF] = f.get();
}

void runOp(){
	operation = mem[prgCounter] << 8 | mem[prgCounter + 1];
	
	Chip8Table[(operation&0xF000) >> 12]();
	prgCounter += 2;
}