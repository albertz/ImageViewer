
#include <SDL.h>
#include <SDL_ttf.h>
#include <list>
#include <map>
#include <iostream>
#include "Font.h"


static auto &errors = std::cerr;
static auto &notes = std::cout;
using std::endl;

static const int CacheLimit = 100;
struct RenderedText;

static int _compare(const std::string& a, const std::string& b) {
	return a.compare(b);
}

static int _compare(Uint8 a, Uint8 b) {
	if(a < b) return -1;
	if(a > b) return 1;
	return 0;
}

#define _CompCheck(Attrib) \
	{ int c = ::_compare(Attrib, other.Attrib); if(c != 0) return c; }

struct Cache {
	struct Key {
		std::string text;
		SDL_Color fg;

		Key() : text(), fg() {}
		Key(const std::string& _t, SDL_Color _fg)
				: text(_t), fg(_fg) {}

		int compare(const Key& other) const {
			_CompCheck(text);
			_CompCheck(fg.r);
			_CompCheck(fg.g);
			_CompCheck(fg.b);
			_CompCheck(fg.a);
			return 0;
		}

		bool operator<(const Key& other) const {
			return compare(other) < 0;
		}
	};

	std::list<RenderedText*> list;
	std::map<Key, RenderedText*> byString;

	void clear();
	void removeBottom();

	~Cache() {
		clear();
	}
};

static Cache cache;

struct RenderedText {
	Cache::Key m_key;
	std::shared_ptr<Texture> m_texture;
	decltype(cache.list.begin()) m_listPtr;

	void moveTop() {
		cache.list.push_front(this);
		cache.list.erase(m_listPtr);
		m_listPtr = cache.list.begin();
	}
};

void Cache::clear() {
	for(RenderedText* r : list)
		delete r;
	list.clear();
	byString.clear();
}

void Cache::removeBottom() {
	assert(!list.empty());
	RenderedText* r = list.back();
	list.pop_back();
	byString.erase(r->m_key);
	delete r;
}


struct Font {
	TTF_Font* m_font;

	Font() : m_font(0) {
		if(TTF_Init() != 0) {
			errors << "TTF_Init failed: " << TTF_GetError() << endl;
			return;
		}

		m_font = TTF_OpenFont("/Library/Fonts/Arial.ttf", 10);
		if(!m_font) {
			errors << "cannot open font: " << TTF_GetError() << endl;
			return;
		}
	}

	~Font() {
		if(m_font)
			TTF_CloseFont(m_font);
		m_font = 0;
	}

	operator bool() const { return m_font != NULL; }
};

static Font font;


std::shared_ptr<Texture> getTextureForText(const std::string& t, SDL_Color fg) {
	Cache::Key key(t, fg);

	auto it = cache.byString.find(key);
	if(it != cache.byString.end()) {
		RenderedText* r = it->second;
		r->moveTop();
		return r->m_texture;
	}

	if(!font) return NULL;

	if(cache.list.size() >= CacheLimit)
		cache.removeBottom();

	Surface surface(TTF_RenderUTF8_Blended(font.m_font, t.c_str(), fg));
	if(!surface) {
		errors << "TTF_RenderUTF8_Blended failed: " << TTF_GetError() << endl;
		return NULL;
	}

	std::shared_ptr<Texture> texture(new Texture(renderer, surface.m_surf));
	if(!*texture) return NULL;

	// put into cache
	RenderedText* r = new RenderedText();
	cache.list.push_front(r);
	cache.byString[key] = r;
	r->m_listPtr = cache.list.begin();
	r->m_key = key;
	r->m_texture = texture;

	return texture;
}
