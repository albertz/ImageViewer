#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <SDL/SDL.h>

static SDL_Window *window;

static void eventLoop() {
	SDL_Event ev;
	while(SDL_WaitEvent(&ev)) {
		switch(ev.type) {
			case SDL_QUIT:
				return;
			default:
				break;
		}
	}
}

int main() {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "error on SDL_Init: " << SDL_GetError() << std::endl;
		return 1;
	}

	window = SDL_CreateWindow(
			"ImageViewer",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			0);
	if(!window) {
		std::cerr << "error on creating window: " << SDL_GetError() << std::endl;
		return 1;
	}

	eventLoop();

	SDL_DestroyWindow(window);

	return 0;
}

