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
#include "Font.h"


static auto &errors = std::cerr;
static auto &notes = std::cout;
using std::endl;

namespace fs = boost::filesystem;

bool quit = false;
bool fullscreen = false;
static SDL_Window *window;
SDL_Renderer* renderer;


struct Picture {
	std::shared_ptr<Texture> m_texture;
	fs::path m_path;

	Picture(const fs::path& path) : m_path(path) {}

	void load() {
		if(*this) return;
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

	operator bool() const { return m_texture.get() && *m_texture; }

	void render() {
		if(!m_texture.get()) return;
		if(!*m_texture) return;

		// TODO: rotate
		// TODO: correct ratio scale
		SDL_RenderCopy(renderer, m_texture->m_texture, 0, 0);

		auto t = getTextureForText(m_path.leaf().string(), ColorWhite());
		if(t.get()) {
			SDL_Rect dstrect;
			dstrect.x = 0;
			dstrect.y = 0;
			dstrect.w = 100;
			dstrect.h = 20;
			SDL_RenderCopy(renderer, t->m_texture, 0, &dstrect);
		}
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
		prepareSelectedPic();
	}

	void nextPic() {
		if(m_curPic == m_pictures.end()) return;
		++m_curPic;
		if(m_curPic == m_pictures.end()) m_curPic = m_pictures.begin();
		prepareSelectedPic();
	}

	void prevPic() {
		if(m_curPic == m_pictures.begin()) {
			if(m_pictures.empty()) return;
			m_curPic = m_pictures.end();
		}
		--m_curPic;
		prepareSelectedPic();
	}

	void prepareSelectedPic() {
		if(m_curPic == m_pictures.end()) return;
		m_curPic->load();
	}

	void render() {
		if(m_curPic == m_pictures.end()) selectPic();
		if(m_curPic == m_pictures.end()) return;
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
		case SDLK_LEFT:
			pictures.prevPic();
			break;
		case SDLK_RIGHT:
			pictures.nextPic();
			break;
		default:
			break;
	}
}

static void mainLoop() {
	while(true) {
		SDL_RenderClear(renderer);
		pictures.render();
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

	if(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF|IMG_INIT_WEBP) == 0) {
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

