#include "application.hpp"
#include "screenPlayerSelect.hpp"
#include "fontPool.hpp"
#include "gui.hpp"

class BasicApp;

class ScreenMainMenu : public Screen
{
    public:

	GLuint background;
	GLuint pressAnyKey;

    ScreenMainMenu(Application* _app);
    ~ScreenMainMenu();

    void update();
    void draw();

    virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player);
    virtual void onSet(string id_orig);
    virtual void onQuit(string id_dest);
};

class BasicApp : public Application
{
public:
    BasicApp(SDL_Window* _window, int _width, int _height);
    ~BasicApp();
};

class ScreenVictory : public Screen
{
	public:

    SDL_Color textColor;
	GUI_TexteDynamique *txtVictory;

	ScreenVictory(BasicApp* _app);
	~ScreenVictory();

	void update();

	void draw();

	virtual void event_KEYDOWN(SDL_Event& event);
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);
};

class ScreenStats : public Screen
{
	public:

    SDL_Color textColor;
	GUI_TexteDynamique *txtStats;

	ScreenStats(Application* _app);
	~ScreenStats();

	void update();

	void draw();

    virtual void event_KEYDOWN(SDL_Event& event);
	virtual void event_MOUSEBUTTONDOWN(SDL_Event& event);
	virtual void onSet(string id_orig);
	virtual void onQuit(string id_dest);
};
