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


static SDL_Window *window;


struct Picture {
	std::shared_ptr<Texture> m_texture;
	fs::path m_path;

	Picture(const fs::path& path) : m_path(path) {}
};

struct Pictures {
	std::list<Picture> m_pictures;

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
};

static Pictures pictures;


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

int main(int argc, char** argv) {
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

	eventLoop();

	SDL_DestroyWindow(window);

	return 0;
}

