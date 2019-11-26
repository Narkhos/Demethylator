#pragma once

#include <SDL.h>
#include "gui.hpp"
#include "application.hpp"
#include "game.hpp"

class ScreenGame : public Screen
{
public:

	Level *level;
	Player *players[4];
	GLuint victoryTexId[4];
	int winner;

	SDL_Color textColor;
	GUI_TexteDynamique *txtGame;

	ScreenGame(Application* _app);
	~ScreenGame();

	void initGame();

	int checkVictory();
	void update();
	void drawVictoryMessage();
	void draw();

	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player);
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);
};
