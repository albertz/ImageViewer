#ifndef __ImageViewer_GFX_H__
#define __ImageViewer_GFX_H__

#include <SDL.h>
#include <boost/noncopyable.hpp>
#include <iostream>

struct Surface : boost::noncopyable {
	SDL_Surface* m_surf;
	Surface() : m_surf(0) {}
	~Surface() {
		if(m_surf)
			SDL_FreeSurface(m_surf);
		m_surf = 0;
	}
};

struct Texture : boost::noncopyable {
	SDL_Texture* m_texture;
	Texture(SDL_Texture* texture = 0) : m_texture(texture) {}
	Texture(SDL_Renderer* renderer, SDL_Surface* surf) {
		m_texture = SDL_CreateTextureFromSurface(renderer, surf);
		if(!m_texture)
			std::cerr << "Error SDL_CreateTextureFromSurface: " << SDL_GetError() << std::endl;
	}
	~Texture() {
		if(m_texture)
			SDL_DestroyTexture(m_texture);
		m_texture = 0;
	}
};


#endif

