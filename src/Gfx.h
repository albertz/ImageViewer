#ifndef __ImageViewer_GFX_H__
#define __ImageViewer_GFX_H__

#include <SDL.h>
#include <boost/noncopyable.hpp>
#include <iostream>
#include <assert.h>
#include <stdint.h>

struct Surface : boost::noncopyable {
	SDL_Surface* m_surf;
	Surface(SDL_Surface* surf = 0) : m_surf(surf) {}
	operator bool() const { return m_surf != NULL; }
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
	operator bool() const { return m_texture != NULL; }
	~Texture() {
		if(m_texture)
			SDL_DestroyTexture(m_texture);
		m_texture = 0;
	}
};

extern SDL_Renderer* renderer;

static inline SDL_Color Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

static inline SDL_Color ColorBlack() { return Color(0,0,0,255); }
static inline SDL_Color ColorWhite() { return Color(255,255,255,255); }

#endif

