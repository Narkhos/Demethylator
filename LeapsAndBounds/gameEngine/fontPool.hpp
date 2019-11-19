#pragma once

#include <SDL_ttf.h>
#include <map>

using namespace std;

// Gestionnaire de chargement/recherche de police de caractère
class FontPool
{
protected:
	map<string, TTF_Font*> fontList;
	string path;

	TTF_Font* loadFont(string filename);
public:

	FontPool(string _path);
	~FontPool();
	TTF_Font* getFont(const std::string& name);
};