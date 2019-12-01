#include "screenGame.hpp"

// class ScreenGame : public Screen

ScreenGame::ScreenGame(Application* _app)
	:Screen(_app, "Game"), level(nullptr)
{
	cout << "Init game screen" << endl;

	for (int i = 0; i < 4; i++) {
		players[i] = nullptr;
	}

	for (int i = 0; i < 4; i++) {
		stringstream ss;
		ss << "win" << i << ".png";
		this->victoryTexId[i] = _app->texturePool->getTexture(ss.str())->texId;
	}

	textColor = { 255 , 255 , 255, 255 };
	txtGame = new GUI_TexteDynamique(L"Running", _app->fontPool->getFont("OxygenMono-Regular.ttf"), textColor);
}
ScreenGame::~ScreenGame() {}

void ScreenGame::initGame()
{
	cout << "Init game" << endl;

	this->winner = -1;

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
	if (this->winner != -1) return this->winner;

	for (int i = 0; i < 4; i++) {
		if (players[i] != nullptr) {

			if (!players[i]->isDead()) {
				if (this->winner != -1) {
					this->winner = -1;
					break;
				}
				else {
					this->winner = i;
				}
			}
		}
	}

	return this->winner;
}

void ScreenGame::update()
{
	if (this->winner != -1 || this->checkVictory() >= 0) return;

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
	if (winner != -1 && (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN)) {
		app->setScreen("Game");
	}

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
	if (this->winner != -1) {
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			app->setScreen("MainMenu");
		}
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

	if (this->winner != -1) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
			app->setScreen("Game");
		}
	}
}

void ScreenGame::drawVictoryMessage()
{
	if (this->winner != -1) {
		drawImage(this->victoryTexId[winner], 440, 260, 400, 200, 1.0f);
	}
}

void ScreenGame::draw()
{
	// Afficher le jeu
	// txtGame->draw(10, 50);

	// DISPLAY

	level->draw(players, app->currentTime);
	this->drawVictoryMessage();
}

void ScreenGame::onSet(string id_orig)
{
	this->initGame();
}

void ScreenGame::onQuit(string id_dest)
{
}
