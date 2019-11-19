#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <config.hpp>
#include <basicApp.hpp>

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

using namespace std;

SDL_Window* window;

int width = 1024;
int height = 768;
int windowedWidth = 1024;
int windowedHeight = 768;
bool fullscreen = false;

string gameTitle = "Leaps and Bounds";

// MAIN
int main(int argc, char* argv[])
{
	Config config("./config.json");
	width = config.data["width"];
	height = config.data["height"];
	fullscreen = config.data["fullscreen"];

	windowedWidth = width;
	windowedHeight = height;

	SDL_GLContext glContext;
	SDL_Event event;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(1);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
	{
		string msg = "SDL failed to initialize: ";
		msg.append(SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
		return 0;
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
	{
		printf("Error initializing SDL_mixer: %s\n", Mix_GetError());
		exit(1);
	}

	int nbMappingsAdded = SDL_GameControllerAddMappingsFromFile("./data/gamecontrollerdb.txt");
	if (nbMappingsAdded == -1)
	{
		cout << SDL_GetError() << endl;
	}
	else
	{
		cout << "Nombre de mappings ajoutes : " << nbMappingsAdded << endl;
	}

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	if (fullscreen) flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
	window = SDL_CreateWindow(gameTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

	if (window == nullptr)
	{
		std::string msg = "Window could not be created: ";
		msg.append(SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
		return 0;
	}

	glContext = SDL_GL_CreateContext(window);

	if (glContext == nullptr)
	{
		std::string msg = "OpenGL context could not be created: ";
		msg.append(SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);

		return 0;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0);
	glViewport(0, 0, width, height);
	glOrtho(0, width, height, 0, -1, 1);

	// Disable vsync for performance purpose
	SDL_GL_SetSwapInterval(0);

	glewInit();
	
	BasicApp app(window, width, height);
	
	app.mainLoop();
	
	SDL_GL_DeleteContext(glContext);
	glContext = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}
