#pragma once

#include <SDL.h>
#include "gui.hpp"
#include "application.hpp"
#include "game.hpp"

class ScreenGame : public Screen
{
public:

	// GLuint background;
	Level *level;
	Player *players[4];

	SDL_Color textColor;
	GUI_TexteDynamique *txtGame;

	ScreenGame(Application* _app);
	~ScreenGame();

	void initGame();

	int checkVictory();
	void update();

	void draw();

	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player);
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);
};
