#include "screenPlayerSelect.hpp"

// class ScreenPlayerSelect : public Screen

ScreenPlayerSelect::ScreenPlayerSelect(Application* app, int _minPlayers, int _maxPlayers)
    :Screen(app,"PlayerSelect"), minPlayers(_minPlayers)
{
    textColor = {255 , 255 , 255, 255};
	txtGame = new GUI_TexteDynamique(L"Player Selection", app->fontPool->getFont("OxygenMono-Regular.ttf"), textColor);

	// Chargement des images de fond correspondant aux différents joueurs
	for(int i = 0; i<_maxPlayers; i++)
    {
        stringstream ss;
        ss << "playerSelect/player" << i+1 << ".png";
        playerBg.push_back(this->app->texturePool->getTexture(ss.str()));
    }

    pressStart = this->app->texturePool->getTexture("playerSelect/pressStart.png");
    ready = this->app->texturePool->getTexture("playerSelect/ready.png");
    go = new GUI_Button(this->app->texturePool->getTexture("playerSelect/go.png")->getId(),
        app->width/2 - 50, app->height-80, 100,40, 2);
}
ScreenPlayerSelect::~ScreenPlayerSelect() {}

void ScreenPlayerSelect::update()
{
    // Bouton Go
    if(app->controllerPool.countPlayersWithController()>= this->minPlayers)
    {
        go->etat=1;
        go->actif=true;
    }
    else
    {
        go->etat=0;
        go->actif=false;
    }
}

void ScreenPlayerSelect::event_KEYDOWN(SDL_Event& event)
{
	if (event.key.keysym.sym == SDLK_ESCAPE) {
		app->setScreen("MainMenu");
		return;
	}

	if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
		int player = app->controllerPool.associateKeyboardToPlayer();
		cout << "Player " << player << " will play on keyboard" << endl;
	}
}

void ScreenPlayerSelect::event_MOUSEBUTTONDOWN(SDL_Event& event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        if(go->actif && go->clique(app->mouse_x, app->mouse_y))
        {
            app->setScreen("Game");
        }
    }
}

void ScreenPlayerSelect::event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player)
{
    if(event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
    {
        this->app->controllerPool.associateControllerToPlayer(event.cbutton.which, true);

        for(int i = 0; i < this->app->controllerPool.playerControllers.size(); i++)
        {
            cout << "Player " << i << " : " << this->app->controllerPool.playerControllers[i] << endl << endl;
        }
    }
}

void ScreenPlayerSelect::onSet(string id_orig)
{
    app->controllerPool.clearControllerToPlayer();
}

void ScreenPlayerSelect::onQuit(string id_dest)
{
    if(id_dest=="Game")
    {
        cout << "C'est parti !" << endl;
    }
    else if(id_dest=="MainMenu")
    {
        cout << "Ah, bon ... " << endl;
    }
}

void ScreenPlayerSelect::draw()
{
    // Afficher le jeu
    txtGame->draw(10,50);

    for(int i = 0; i < playerBg.size();i++)
    {
        drawImage(playerBg[i]->getId(),i*(app->width/playerBg.size()),0,app->width/playerBg.size(), 4.0/5.0*app->height,1.0,1.0);

        if(i >= app->controllerPool.playerControllers.size() || app->controllerPool.playerControllers[i]==-1)
        {
            drawImage(pressStart->getId(),
                (1.0/12.0+i)*(app->width/playerBg.size()),
                6.0/10.0 * app->height,
                10.0/12.0*app->width/playerBg.size(),
                1.0/12.0*app->height,
                1.0/2.0*sin((float)(app->currentTime)/200.0)+1.0/2.0,
                1.0);
        }
        else
        {
            // Le joueur est pret (TODO voir comment on gère les éventuels paramètres, notamment le choix du personnage s'il y a lieu)
            drawImage(ready->getId(),
                (1.0/12.0+i)*(app->width/playerBg.size()),
                6.0/10.0 * app->height,
                10.0/12.0*app->width/playerBg.size(),
                1.0/12.0*app->height,
                1.0,
                1.0);
        }
    }

    go->draw();
}
