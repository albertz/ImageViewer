#ifndef __ImageViewer_Font_h__
#define __ImageViewer_Font_h__

#include <memory>
#include <string>
#include <SDL.h>
#include "Gfx.h"

std::shared_ptr<Texture> getTextureForText(const std::string& t, SDL_Color fg);

#endif
