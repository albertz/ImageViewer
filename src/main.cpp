#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <boost/noncopyable.hpp>
#include "SmartPointer.h"


static SDL_Window *window;


template <> void SmartPointer_ObjectDeinit<SDL_Surface> ( SDL_Surface * obj ) {
	SDL_FreeSurface(obj);
}
template <> void SmartPointer_ObjectDeinit<SDL_Texture> ( SDL_Texture * obj ) {
	SDL_DestroyTexture(obj);
}
template <> void SmartPointer_ObjectDeinit<SDL_Renderer> ( SDL_Renderer * obj ) {
	SDL_DestroyRenderer(obj);
}
template <> void SmartPointer_ObjectDeinit<SDL_Window> ( SDL_Window * obj ) {
	SDL_DestroyWindow(obj);
}


struct Surface : boost::noncopyable {
	SDL_Surface* m_surf;
	Surface() : m_surf(0) {}
	~Surface() {
		if(m_surf)
			SDL_FreeSurface(m_surf);
		m_surf = 0;
	}
};


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

