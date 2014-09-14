#include <SDL.h>
#include "SmartPointer.h"


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
