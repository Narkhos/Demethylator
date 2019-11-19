#include "basicApp.hpp"

// class BasicApp : public Application

BasicApp::BasicApp(SDL_Window* _window, int _width, int _height)
		:Application(_window, _width, _height)
{
    screenList.push_back(new ScreenMainMenu(this));
    screenList.push_back(new ScreenPlayerSelect(this, 1, 4));
    screenList.push_back(new ScreenGame(this));
    screenList.push_back(new ScreenVictory(this));
    screenList.push_back(new ScreenStats(this));
}
BasicApp::~BasicApp()
{
}

// class ScreenMainMenu : public Screen

ScreenMainMenu::ScreenMainMenu(Application* _app)
    :Screen(_app,"MainMenu")
{
    textColor = {255 , 255 , 255, 255};
	txtMainMenu = new GUI_TexteDynamique(L"Main Menu", _app->fontPool->getFont("OxygenMono-Regular.ttf"), textColor);
}

ScreenMainMenu::~ScreenMainMenu()
{
}

void ScreenMainMenu::update()
{

}

void ScreenMainMenu::event_KEYDOWN(SDL_Event& event)
{
    app->setScreen("PlayerSelect");
}

void ScreenMainMenu::draw()
{
    txtMainMenu->draw(10,50);
}

void ScreenMainMenu::onSet(string id_orig) {}
void ScreenMainMenu::onQuit(string id_dest) {}

// class ScreenVictory : public Screen

ScreenVictory::ScreenVictory(BasicApp* _app)
    :Screen(_app,"Victory")
{
    textColor = {255 , 255 , 255, 255};
	txtVictory = new GUI_TexteDynamique(L"Victory", _app->fontPool->getFont("OxygenMono-Regular.ttf"), textColor);
}

ScreenVictory::~ScreenVictory() {}

void ScreenVictory::update()
{

}


void ScreenVictory::event_KEYDOWN(SDL_Event& event)
{
    app->setScreen("MainMenu");
}

void ScreenVictory::draw()
{
    // Afficher l'�cran de victoire
    txtVictory->draw(10,50);
}

void ScreenVictory::onSet(string id_orig)
{

}

void ScreenVictory::onQuit(string id_dest)
{
}

// class ScreenStats : public Screen

ScreenStats::ScreenStats(Application* _app)
    :Screen(_app,"Stats")
{
    textColor = {255 , 255 , 255, 255};
	txtStats = new GUI_TexteDynamique(L"Stats", _app->fontPool->getFont("OxygenMono-Regular.ttf"), textColor);
}
ScreenStats::~ScreenStats() {}

void ScreenStats::update()
{

}

void ScreenStats::event_KEYDOWN(SDL_Event& event)
{
    app->setScreen("MainMenu");
}

void ScreenStats::event_MOUSEBUTTONDOWN(SDL_Event& event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        app->setScreen("Game");
    }
    else
    {
        app->setScreen("Victory");
    }
}

void ScreenStats::draw()
{
    // Afficher les stats
    txtStats->draw(10,50);
}

void ScreenStats::onSet(string id_orig)
{

}

void ScreenStats::onQuit(string id_dest)
{
}
