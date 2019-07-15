#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;

constexpr int scrWidth = 8 * 64, scrHeight = 8 * 32;

#include "emu.h"

constexpr uint8_t sdlKeys[16] = {
	SDLK_x,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_z,
	SDLK_c,
	SDLK_4,
	SDLK_r,
	SDLK_f,
	SDLK_v,
};

void eventLoop() {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			killSwitch = true;
		}

		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {

			case SDLK_ESCAPE:
				killSwitch = true;
				break;

			default:
				break;
			}
			
			for (int i = 0; i < 16; ++i) {
				if (event.key.keysym.sym == sdlKeys[i]) {
					keys[i] = 1;
				}
			}
		}
		
		if (event.type == SDL_KEYUP) {
			for (int i = 0; i < 16; ++i) {
				if (event.key.keysym.sym == sdlKeys[i]) {
					keys[i] = 0;
				}
			}
		}
	}
}

int main(int argc, char *argv[]){
	
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		std::cerr << "SDL2 could not initialize!" << std::endl;
		std::cerr << SDL_GetError() << std::endl;
		return -1;
	}
	
	std::cout << "Starting up Cookie Emulator Deluxe !" << std::endl;
	
	window = SDL_CreateWindow("CookieEmu Deluxe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scrWidth, scrHeight, SDL_WINDOW_SHOWN);
	if(window == NULL){
		std::cerr << "Window could not initialize!" << std::endl;
		std::cerr << SDL_GetError() << std::endl;
		return -1;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "renderer could not initialize:" << SDL_GetError() << std::endl;
	}
	
	SDL_RenderSetLogicalSize(renderer, scrWidth, scrHeight);
	
	initEmulator();
	
	load("roms/maze.cp8");
	
	SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			64, 32);
			
	Uint32 pixels[64 * 32];

	while (!killSwitch) {
		eventLoop();
		runOp();
		
		if (drawn) {
			drawn = false;

			for (int i = 0; i < 64 * 32; ++i) {
				uint8_t pixel = screen[i];
				pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
			}

			SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
		
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}	
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}