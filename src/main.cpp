#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <list>
#include <string>
#include "SmartPointer.h"

static auto& errors = std::cerr;
using std::endl;


static SDL_Window *window;




static std::list<std::string> pictureList;

static void buildPictureList() {
	namespace fs = boost::filesystem;
}

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
		errors << "error on SDL_Init: " << SDL_GetError() << endl;
		return 1;
	}

	window = SDL_CreateWindow(
			"ImageViewer",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			0);
	if(!window) {
		errors << "error on creating window: " << SDL_GetError() << endl;
		return 1;
	}

	eventLoop();

	SDL_DestroyWindow(window);

	return 0;
}

