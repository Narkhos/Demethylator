#include "screenGame.hpp"

// class ScreenGame : public Screen

ScreenGame::ScreenGame(Application* _app)
	:Screen(_app, "Game"), level(nullptr)
{
	cout << "Init game screen" << endl;

	for (int i = 0; i < 4; i++) {
		players[i] = nullptr;
	}

	textColor = { 255 , 255 , 255, 255 };
	txtGame = new GUI_TexteDynamique(L"Running", _app->fontPool->getFont("OxygenMono-Regular.ttf"), textColor);
}
ScreenGame::~ScreenGame() {}

void ScreenGame::initGame()
{
	cout << "Init game" << endl;
	if (level != nullptr) {
		delete level;
	}

	level = new Level(levelString, 0, 0, 1280, 720);

	for (int i = 0; i < 4; i++) {
		delete players[i];
		players[i] = nullptr;

		if (app->controllerPool.getController(i) != -1) {
			players[i] = new Player(level->playerSpawns[i].x, level->playerSpawns[i].y, 3, REGRESS, i);
		}
	}
}

int ScreenGame::checkVictory()
{
	int winner = -1;
	for (int i = 0; i < 4; i++) {
		if (players[i] != nullptr) {
			
			if (!players[i]->isDead()) {
				if (winner != -1) {
					winner = -1;
					break;
				}
				else {
					winner = i;
				}
			}
		}
	}

	return winner;
}

void ScreenGame::update()
{
	if (this->checkVictory() >= 0) {
		app->setScreen("Stats");
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (players[i] != nullptr) players[i]->update(app->deltaTime, level->position, level->w, level->h);
	}

	level->update(app->deltaTime, players);

	// INPUTS
	for (int i = 0; i < app->controllerPool.playerControllers.size(); i++) {
		int controllerId = app->controllerPool.playerControllers[i];

		if (controllerId == -1) continue;

		if (controllerId == KEYBOARD_CONTROLLER_ID) {
			const Uint8* keyState = SDL_GetKeyboardState(nullptr);

			if (keyState[SDL_SCANCODE_W]) {
				players[i]->setSpeedY(-1.0f);
			}

			if (keyState[SDL_SCANCODE_S]) {
				players[i]->setSpeedY(1.0f);
			}

			if (keyState[SDL_SCANCODE_A]) {
				players[i]->setSpeedX(-1.0f);
			}

			if (keyState[SDL_SCANCODE_D]) {
				players[i]->setSpeedX(1.0f);
			}

			if (keyState[SDL_SCANCODE_UP]) {
				level->spawnBullet(players[i], glm::vec2(0.0, -1.0));
			}

			if (keyState[SDL_SCANCODE_DOWN]) {
				level->spawnBullet(players[i], glm::vec2(0.0, 1.0));
			}

			if (keyState[SDL_SCANCODE_LEFT]) {
				level->spawnBullet(players[i], glm::vec2(-1.0, 0.0));
			}

			if (keyState[SDL_SCANCODE_RIGHT]) {
				level->spawnBullet(players[i], glm::vec2(1.0, 0.0));
			}
		}
		else {
			SDL_GameController *gc = app->controllerPool.controllers[controllerId];


			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1) {
				players[i]->setSpeedY(-1.0f);
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1) {
				players[i]->setSpeedY(1.0f);
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1) {
				players[i]->setSpeedX(-1.0f);
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1) {
				players[i]->setSpeedX(1.0f);
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y) == 1) {
				level->spawnBullet(players[i], glm::vec2(0.0, -1.0));
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A) == 1) {
				level->spawnBullet(players[i], glm::vec2(0.0, 1.0));
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_X) == 1) {
				level->spawnBullet(players[i], glm::vec2(-1.0, 0.0));
			}

			if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B) == 1) {
				level->spawnBullet(players[i], glm::vec2(1.0, 0.0));
			}
		}

	}
}

void ScreenGame::event_KEYDOWN(SDL_Event& event)
{
	if (event.key.keysym.sym == SDLK_ESCAPE) {
		app->setScreen("MainMenu");
		return;
	}
	int player = app->controllerPool.getPlayerFromController(KEYBOARD_CONTROLLER_ID);

	if (player != -1) {
		if (event.key.keysym.sym == SDLK_SPACE)
		{
			players[player]->toggleWeapon();
		}
	}


}

void ScreenGame::event_MOUSEBUTTONDOWN(SDL_Event& event)
{
	if (event.button.button == SDL_BUTTON_LEFT)
	{
		app->setScreen("Stats");
	}
}

void ScreenGame::event_CONTROLLERBUTTONDOWN(SDL_Event &event, int player)
{
	if (player != -1)
	{
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
		{
			players[player]->toggleWeapon();
		}
	}
}

void ScreenGame::draw()
{
	// Afficher le jeu
	// txtGame->draw(10, 50);

	// DISPLAY

	level->draw(players, app->currentTime);
}

void ScreenGame::onSet(string id_orig)
{
	this->initGame();
}

void ScreenGame::onQuit(string id_dest)
{
}
