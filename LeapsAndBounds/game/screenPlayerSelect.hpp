#ifndef _SCREEN_PLAYER_SELECT_HPP_
#define _SCREEN_PLAYER_SELECT_HPP_

#include "application.hpp"
#include "gui.hpp"

class ScreenPlayerSelect : public Screen
{
	public:

    SDL_Color textColor;
	GUI_TexteDynamique *txtGame;
	int minPlayers;
    vector<Texture*> playerBg;
    Texture* pressStart;
    Texture* ready;
    GUI_Button* go;

	ScreenPlayerSelect(Application* app, int minPlayers, int _maxPlayers);
	~ScreenPlayerSelect();

	void update();

	void draw();
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);

	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player);
};

#endif
