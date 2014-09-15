#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <memory>
#include <list>
#include <string>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <fstream>
#include "SmartPointer.h"
#include "Gfx.h"

static auto &errors = std::cerr;
static auto &notes = std::cout;
using std::endl;

namespace fs = boost::filesystem;

bool quit = false;
bool fullscreen = false;
static SDL_Window *window;
static SDL_Renderer* renderer;


struct Picture {
	std::shared_ptr<Texture> m_texture;
	fs::path m_path;

	Picture(const fs::path& path) : m_path(path) {}

	void load() {
		Surface surf(IMG_Load(m_path.string().c_str()));
		if(!surf) {
			errors << "cannot load " << m_path << ": " << IMG_GetError() << endl;
			return;
		}
		m_texture.reset(new Texture(renderer, surf.m_surf));
		if(!*m_texture) {
			errors << "cannot create texture from surface: " << SDL_GetError() << endl;
			return;
		}
	}

	void render() {
		if(!m_texture.get()) return;
		if(!*m_texture) return;
		SDL_RenderCopy(renderer, m_texture->m_texture, NULL, NULL);
	}
};

struct Pictures {
	std::list<Picture> m_pictures;
	decltype(m_pictures.begin()) m_curPic;

	void addPicture(const Picture& pic) {
		notes << "Add picture: " << pic.m_path << endl;
		m_pictures.push_back(pic);
	}

	void loadFromList(const fs::path& f) {
		std::ifstream ifs;
		ifs.open(f.string(), std::ios::in);
		if(!ifs) {
			errors << "cannot open " << f << endl;
			return;
		}
		while(!ifs.eof() && !ifs.fail()) {
			std::string s;
			std::getline(ifs, s);
			boost::trim(s);
			if(s.empty()) continue;
			fs::path path = f.parent_path() / fs::path(s);
			if(fs::is_regular_file(path))
				addPicture(Picture(path));
			else
				errors << "file does not exist: " << path.string() << endl;
		}
	}

	void loadDir(const fs::path& dir) {
		for(fs::directory_iterator dir_iter(dir); dir_iter != fs::directory_iterator(); ++dir_iter) {
			if(fs::is_regular_file(dir_iter->status())) {
				fs::path path = dir_iter->path();
				std::string ext = path.extension().string();
				boost::to_lower(ext);
				if(ext == ".jpg" || ext == ".jpeg")
					addPicture(Picture(path));
			}
		}
	}

	void selectPic() {
		m_curPic = m_pictures.begin();
	}

	void render() {
		if(m_curPic != m_pictures.end())
			selectPic();
		if(m_curPic != m_pictures.end())
			return;
		m_curPic->render();
	}
};

static Pictures pictures;


static void onKeyDown(SDL_KeyboardEvent& ev) {
	switch(ev.keysym.sym) {
		case SDLK_ESCAPE:
		case 'q':
			quit = true;
			break;
		case 'f':
			fullscreen = !fullscreen;
			SDL_SetWindowFullscreen(
					window,
					fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
			break;
	}
}

static void mainLoop() {
	while(true) {
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);

		SDL_Event ev;
		if(SDL_WaitEvent(&ev) == 0)
			break;

		switch(ev.type) {
			case SDL_KEYDOWN:
				onKeyDown(ev.key);
				break;
			case SDL_QUIT:
				return;
			default:
				break;
		}
		if(quit) return;
	}
}

int main(int argc, char** argv) {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		errors << "SDL_Init failed: " << SDL_GetError() << endl;
		return 1;
	}

	if(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF|IMG_INIT_WEBP) != 0) {
		errors << "IMG_Init failed: " << IMG_GetError() << endl;
		return 1;
	}

	window = SDL_CreateWindow(
			"ImageViewer",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			0);
	if(!window) {
		errors << "cannot create window: " << SDL_GetError() << endl;
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		errors << "cannot create renderer: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_RenderClear(renderer);

	if(argc > 1) {
		fs::path path = argv[1];
		if(fs::is_regular_file(path))
			pictures.loadFromList(path);
		else if(fs::is_directory(path))
			pictures.loadDir(path);
		else {
			errors << "not found: " << path.string() << endl;
			return 1;
		}
	}
	else
		pictures.loadDir(".");
	pictures.selectPic();

	mainLoop();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}

