#include "application.hpp"
#include <GL/glew.h>

// class Application

Application::Application(SDL_Window* _window, int _width, int _height)
	: window(_window),
		width(_width), height(_height),
		currentWidth(_width), currentHeight(_height),
		currentScreen(0), running(true)
{
	this->fontPool = new FontPool("./data/fonts/");
	this->texturePool = new TexturePool("./data/images/");
	music=nullptr;
    currentMusicFile = "";
}

Application::~Application()
{

}

void Application::mainLoop()
{
	while(this->running)
    {
        // ACTUALISATION DU CONTEXT
        this->update();

		if(currentScreen >= 0)
		{
			if(screenList[currentScreen] != nullptr)
			{
				screenList[currentScreen]->update();

				if(currentScreen >= 0)
                {
                    screenList[currentScreen]->eventManagement();

                    glClear(GL_COLOR_BUFFER_BIT);

                    screenList[currentScreen]->draw();

                    glFlush();
                    SDL_GL_SwapWindow(this->window);
                }
			}
			else
			{
				cout << "L'ecran " << currentScreen << " n'a pas ete trouvé" << endl;
				this->running = false;
			}
		}
		else
		{
			this->running = false;
		}

	}
}

Screen* Application::setScreen(string _id)
{
	for(int i = 0; i < screenList.size(); ++i)
	{
		if(screenList[i] != nullptr && screenList[i]->id == _id)
		{
            Screen* tmp = this->getCurrentScreen();
            string id_orig = "";
            if(tmp!=nullptr)
            {
                tmp->onQuit(_id);
                id_orig = tmp->id;
            }

			this->currentScreen = i;
			screenList[i]->onSet(id_orig);
			return screenList[i];
		}
	}
	return nullptr;
}

Screen* Application::setScreen(int _id)
{
    if(_id >= 0 && _id < screenList.size())
    {
        Screen* tmp = this->getCurrentScreen();
        string id_orig = "";
        if(tmp!=nullptr)
        {
            tmp->onQuit(screenList[_id]->id);
            id_orig = tmp->id;
        }

        currentScreen = _id;
        screenList[currentScreen]->onSet(id_orig);
        return screenList[currentScreen];
    }
    else
    {
        currentScreen = -1;
        return nullptr;
    }
}

Screen* Application::getScreen(string _id)
{
	for(int i = 0; i < screenList.size(); ++i)
	{
		if(screenList[i] != nullptr && screenList[i]->id == _id)
		{
			return screenList[i];
		}
	}
	return nullptr;
}

Screen* Application::getCurrentScreen()
{
	if(currentScreen>=0 && currentScreen < screenList.size())
	{
		return screenList[currentScreen];
	}
	else
	{
		return nullptr;
	}
}

void Application::setMusic(string musicFile)
{
	if (musicFile == "") {
		
		Mix_FreeMusic(music);	
		music = nullptr;

		return;
	}

    if(this->currentMusicFile != musicFile || this->music == nullptr)
    {
        this->currentMusicFile = musicFile;
        
		
		if (music != nullptr) {
			Mix_FreeMusic(music);	
			music = nullptr;
		}

		music = Mix_LoadMUS(musicFile.c_str());

		if (music != nullptr)
		{
			Mix_PlayMusic(music, -1);
			Mix_VolumeMusic(64); // TODO : allow to set music volume
		}
    }
}

void Application::update()
{
	currentTime = SDL_GetTicks();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

    SDL_GetMouseState(&mouse_x,&mouse_y);

    // on ramène la position de la souris aux dimension courantes de la fenêtre
    // pour le cas ou la taille de la fenêtre est différente des dimensions natives de l'application
    mouse_x = mouse_x * this->width / this->currentWidth;
	mouse_y = mouse_y * this->height / this->currentHeight;
}


// class Screen

Screen::Screen(Application* _app, string _id)
	:app(_app), id(_id)
{

}

Screen::~Screen()
{

}

void Screen::eventManagement()
{
    // GESTION DES EVENEMENTS
    SDL_Event event;
    while( SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_AUDIODEVICEADDED :
                event_AUDIODEVICEADDED(event);
            break;
            case SDL_AUDIODEVICEREMOVED :
                event_AUDIODEVICEREMOVED(event);
            break;
            case SDL_CONTROLLERAXISMOTION :
                event_CONTROLLERAXISMOTION(event, app->controllerPool.getPlayerFromController(event.cbutton.which));
            break;
            case SDL_CONTROLLERBUTTONDOWN :
                event_CONTROLLERBUTTONDOWN(event, app->controllerPool.getPlayerFromController(event.cbutton.which));
            break;
            case SDL_CONTROLLERBUTTONUP :
                event_CONTROLLERBUTTONUP(event, app->controllerPool.getPlayerFromController(event.cbutton.which));
            break;
            case SDL_CONTROLLERDEVICEADDED :
                event_CONTROLLERDEVICEADDED(event);
            break;
            case SDL_CONTROLLERDEVICEREMOVED :
                event_CONTROLLERDEVICEREMOVED(event);
            break;
            case SDL_CONTROLLERDEVICEREMAPPED :
                event_CONTROLLERDEVICEREMAPPED(event);
            break;
            case SDL_DOLLARGESTURE :
                event_DOLLARGESTURE(event);
            break;
            case SDL_DOLLARRECORD :
                event_DOLLARRECORD(event);
            break;
            case SDL_DROPFILE :
                event_DROPFILE(event);
            break;
            case SDL_DROPTEXT :
                event_DROPTEXT(event);
            break;
            case SDL_DROPBEGIN :
                event_DROPBEGIN(event);
            break;
            case SDL_DROPCOMPLETE :
                event_DROPCOMPLETE(event);
            break;
            case SDL_FINGERMOTION :
                event_FINGERMOTION(event);
            break;
            case SDL_FINGERDOWN :
                event_FINGERDOWN(event);
            break;
            case SDL_FINGERUP :
                event_FINGERUP(event);
            break;
            case SDL_KEYDOWN :
                event_KEYDOWN(event);
            break;
            case SDL_KEYUP :
                event_KEYUP(event);
            break;
            case SDL_JOYAXISMOTION :
                event_JOYAXISMOTION(event);
            break;
            case SDL_JOYBALLMOTION :
                event_JOYBALLMOTION(event);
            break;
            case SDL_JOYHATMOTION :
                event_JOYHATMOTION(event);
            break;
            case SDL_JOYBUTTONDOWN :
                event_JOYBUTTONDOWN(event);
            break;
            case SDL_JOYBUTTONUP :
                event_JOYBUTTONUP(event);
            break;
            case SDL_JOYDEVICEADDED :
                event_JOYDEVICEADDED(event);
            break;
            case SDL_JOYDEVICEREMOVED :
                event_JOYDEVICEREMOVED(event);
            break;
            case SDL_MOUSEMOTION :
                event_MOUSEMOTION(event);
            break;
            case SDL_MOUSEBUTTONDOWN :
                event_MOUSEBUTTONDOWN(event);
            break;
            case SDL_MOUSEBUTTONUP :
                event_MOUSEBUTTONUP(event);
            break;
            case SDL_MOUSEWHEEL :
                event_MOUSEWHEEL(event);
            break;
            case SDL_MULTIGESTURE :
                event_MULTIGESTURE(event);
            break;
            case SDL_QUIT :
                event_QUIT(event);
            break;
            case SDL_SYSWMEVENT :
                event_SYSWMEVENT(event);
            break;
            case SDL_TEXTEDITING :
                event_TEXTEDITING(event);
            break;
            case SDL_TEXTINPUT :
                event_TEXTINPUT(event);
            break;
            case SDL_USEREVENT :
                event_USEREVENT(event);
            break;
            case SDL_WINDOWEVENT :
                event_WINDOWEVENT(event);
            break;
            default:
            break;
        }
    }
}

void Screen::event_AUDIODEVICEADDED(SDL_Event &event){ }
void Screen::event_AUDIODEVICEREMOVED(SDL_Event &event){ }
void Screen::event_CONTROLLERAXISMOTION(SDL_Event &event, int player){ }

void Screen::event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player)
{

}

void Screen::event_CONTROLLERBUTTONUP(SDL_Event &event, int player)
{
}

void Screen::event_CONTROLLERDEVICEADDED(SDL_Event &event)
{
    int instanceID = app->controllerPool.addController( event.cdevice.which );

	// Si on trouve un joueur sans controller au branchement, on lui affecte la manette
    app->controllerPool.associateControllerToPlayer(instanceID, false);
}

void Screen::event_CONTROLLERDEVICEREMOVED(SDL_Event &event)
{
    app->controllerPool.removeController( event.cdevice.which );
}

void Screen::event_CONTROLLERDEVICEREMAPPED(SDL_Event &event){ }
void Screen::event_DOLLARGESTURE(SDL_Event &event){ }
void Screen::event_DOLLARRECORD(SDL_Event &event){ }
void Screen::event_DROPFILE(SDL_Event &event){ }
void Screen::event_DROPTEXT(SDL_Event &event){ }
void Screen::event_DROPBEGIN(SDL_Event &event){ }
void Screen::event_DROPCOMPLETE(SDL_Event &event){ }
void Screen::event_FINGERMOTION(SDL_Event &event){ }
void Screen::event_FINGERDOWN(SDL_Event &event){ }
void Screen::event_FINGERUP(SDL_Event &event){ }
void Screen::event_KEYDOWN(SDL_Event &event){ }
void Screen::event_KEYUP(SDL_Event &event){ }
void Screen::event_JOYAXISMOTION(SDL_Event &event){ }
void Screen::event_JOYBALLMOTION(SDL_Event &event){ }
void Screen::event_JOYHATMOTION(SDL_Event &event){ }
void Screen::event_JOYBUTTONDOWN(SDL_Event &event){ }
void Screen::event_JOYBUTTONUP(SDL_Event &event){ }
void Screen::event_JOYDEVICEADDED(SDL_Event &event){ }
void Screen::event_JOYDEVICEREMOVED(SDL_Event &event){ }
void Screen::event_MOUSEMOTION(SDL_Event &event){ }
void Screen::event_MOUSEBUTTONDOWN(SDL_Event &event){ }
void Screen::event_MOUSEBUTTONUP(SDL_Event &event){ }
void Screen::event_MOUSEWHEEL(SDL_Event &event){ }
void Screen::event_MULTIGESTURE(SDL_Event &event){ }

void Screen::event_QUIT(SDL_Event &event)
{
    app->running = false;
}

void Screen::event_SYSWMEVENT(SDL_Event &event){ }
void Screen::event_TEXTEDITING(SDL_Event &event){ }
void Screen::event_TEXTINPUT(SDL_Event &event){ }
void Screen::event_USEREVENT(SDL_Event &event){ }

void Screen::event_WINDOWEVENT(SDL_Event &event)
{
    if( event.window.event == SDL_WINDOWEVENT_RESIZED )
    {
        glViewport( 0, 0, event.window.data1, event.window.data2 );
        glLoadIdentity();

        glOrtho(0, this->app->width, this->app->height, 0, -1, 1);
        this->app->currentWidth = event.window.data1;
        this->app->currentHeight = event.window.data2;
    }
}
