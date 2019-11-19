#include "fontPool.hpp"
#include <iostream>
#include <sstream>

using namespace std;

// class FontPool

TTF_Font* FontPool::loadFont(string filepath)
{
	// INITIALISATION de SDL_ttf
	if (!TTF_WasInit() && TTF_Init() == -1)
	{
		cerr << "TTF_Init : " << TTF_GetError() << endl;
		return nullptr;
	}

	TTF_Font* font = TTF_OpenFont(filepath.c_str(), 16);

	if (!font)
	{
		cerr << "TTF_OpenFont : " << TTF_GetError() << endl;
		return nullptr;
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	return font;
}

FontPool::FontPool(string _path) :path(_path)
{
}

FontPool::~FontPool()
{
	map<string, TTF_Font*>::iterator p;
	for (p = fontList.begin(); p != fontList.end(); p++)
	{
		if (p->second != nullptr) TTF_CloseFont(p->second);
		p->second = nullptr;

	}
}

TTF_Font* FontPool::getFont(const std::string& fichier)
{
	stringstream ss;
	ss << this->path << fichier;

	if (fontList.count(fichier) == 0)
	{
		fontList[ss.str()] = this->loadFont(ss.str());
	}

	return fontList[ss.str()];
}
