#include "game.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <glm/glm.hpp>
#include "gui.hpp"

using namespace std;

bool showFPS = false;
TexturePool texturePool("./data/images/");

// GAME

// class Hitbox

Hitbox::Hitbox()
	:position(glm::vec2(0.0, 0.0)), width(1.0), height(1.0), group("")
{
	this->texId = texturePool.getTexture("hitbox.png")->texId;
}

Hitbox::Hitbox(glm::vec2 _position, float _width, float _height, string _group)
	: position(_position), width(_width), height(_height), group(_group)
{
	this->texId = texturePool.getTexture("hitbox.png")->texId;
}

Hitbox::~Hitbox()
{

}

void Hitbox::update(glm::vec2 newPosition, float newWidth, float newHeight)
{
	this->position = newPosition;
	this->width = newWidth;
	this->height = newHeight;
}

bool Hitbox::collision(Hitbox& hitbox)
{
	return (this->position.x < hitbox.position.x + hitbox.width &&
		this->position.x + this->width > hitbox.position.x &&
		this->position.y < hitbox.position.y + hitbox.height &&
		this->height + this->position.y > hitbox.position.y);
}

void Hitbox::draw()
{
	if (showFPS) drawImage(this->texId, this->position.x, this->position.y, this->width, this->height, 1.0f);
}


// class Wall

Wall::Wall(int _column, int _line, float x, float y, float _w, float _h)
	:column(_column), line(_line), position(x, y), w(_w), h(_h)
{
	this->texId = texturePool.getTexture("wall.png")->texId;
	initTexCoord(0);
}

Wall::Wall(int _column, int _line, glm::vec2 _position, float _w, float _h)
	:column(_column), line(_line), position(_position), w(_w), h(_h)
{
	this->texId = texturePool.getTexture("wall.png")->texId;
	initTexCoord(0);
}

Wall::~Wall() {}

void Wall::initTexCoord(int neighbourCode)
{
	float offset = neighbourCode / 15.0f;

	texCoord[0] = neighbourCode / 15.0f;
	texCoord[1] = 0.0;

	texCoord[2] = (neighbourCode + 1.0f) / 15.0f;
	texCoord[3] = 0.0;

	texCoord[4] = (neighbourCode + 1.0f) / 15.0f;
	texCoord[5] = 1.0;

	texCoord[6] = neighbourCode / 15.0f;
	texCoord[7] = 1.0;
}

void Wall::draw()
{
	drawImage(this->texId, position.x, position.y, this->w, this->h, 1.0f, 1.0f, texCoord);
}


// class Mushroom

Mushroom::Mushroom(float x, float y, float _w, float _h, bool _bonus)
	:position(x, y), w(_w), h(_h), bonus(_bonus ? 1 : -1)
{
	this->texId = texturePool.getTexture(_bonus ? "redmushroom.png" : "greenmushroom.png")->texId;
	this->hitbox = Hitbox(position + glm::vec2(0, h / 2), w, h / 2, "bonus");
}

Mushroom::Mushroom(glm::vec2 _position, float _w, float _h, bool _bonus)
	:position(_position), w(_w), h(_h), bonus(_bonus ? 1 : -1)
{
	this->texId = texturePool.getTexture(_bonus ? "redmushroom.png" : "greenmushroom.png")->texId;
	this->hitbox = Hitbox(position + glm::vec2(0, h / 2), w, h / 2, "bonus");
}

Mushroom::~Mushroom() {}

void Mushroom::draw()
{
	drawImage(this->texId, position.x, position.y, this->w, this->h, 1.0f);
	hitbox.draw();
}

PlayerState playerState[7] = {
	// Dead
	{
		glm::vec2(0, 0),	// offset
		1.0f,				// zoom
		glm::vec2(6, 4),	// hitboxOffset
		16.0f,				// hitboxWidth
		30.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(0, 0),	// offset
		1.0f,				// zoom
		glm::vec2(6, 4),	// hitboxOffset
		16.0f,				// hitboxWidth
		30.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-15, -15),// offset
		2.0f,				// zoom
		glm::vec2(0, -11),	// hitboxOffset
		32.0f,				// hitboxWidth
		64.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-31, -31),	// offset
		3.0f,				// zoom
		glm::vec2(-8, -31),	// hitboxOffset
		48.0f,				// hitboxWidth
		96.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-47, -47),	// offset
		4.0f,				// zoom
		glm::vec2(-15, -47),	// hitboxOffset
		64.0f,				// hitboxWidth
		128.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-63, -63),	// offset
		5.0f,				// zoom
		glm::vec2(-25, -63),	// hitboxOffset
		80.0f,				// hitboxWidth
		160.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	// Dead
	{
		glm::vec2(0, 0),	// offset
		1.0f,				// zoom
		glm::vec2(6, 4),	// hitboxOffset
		16.0f,				// hitboxWidth
		30.0f,				// hitboxHeight
		1.00f,				// maxSpeed
		glm::vec2(15, 15),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	}
};

// class Player
Player::Player(float x, float y, int _state, BulletType _weaponType, int _playerId)
	:position(glm::vec2(x, y)), precPosition(glm::vec2(x, y)), speed(glm::vec2(0.0, 0.0)), weaponType(_weaponType),
	gunCooldown(0), playerId(_playerId), left(false)
{
	stringstream ssWalk;
	ssWalk << playerId << "/walk.png";
	walk = new BasicAnimation2d(ssWalk.str(), 100, texturePool);

	stringstream ssIdle;
	ssIdle << playerId << "/idle.png";
	idle = new BasicAnimation2d(ssIdle.str(), 2000, texturePool);

	stringstream ssDead;
	ssDead << playerId << "/dead.png";
	dead = new BasicAnimation2d(ssDead.str(), 2000, texturePool);

	this->setState(_state);
}

Player::~Player()
{
}

void Player::setState(int _state)
{
	stringstream hitboxGroup;
	this->state = _state;

	hitboxGroup << "player" << playerId;
	this->hitbox = Hitbox(position + playerState[state].hitboxOffset, playerState[state].hitboxWidth, playerState[state].hitboxHeight, hitboxGroup.str());
	this->precHitbox = this->hitbox;
}

void Player::setSpeedX(float value)
{
	this->speed.x = value * playerState[state].maxSpeed;
}

void Player::setSpeedY(float value)
{
	this->speed.y = value * playerState[state].maxSpeed;
}

void Player::evolve()
{
	if (!this->isDead()) {
		this->setState(min(this->state + 1, 6));
	}
}

void Player::degenerate()
{
	if (!this->isDead()) {
		this->setState(max(this->state - 1, 0));
	}
}

void Player::evolveOrRegress(BulletType type) {
	if (type == EVOLVE) {
		this->evolve();
	}
	else {
		this->degenerate();
	}
}

Hitbox Player::getHitbox()
{
	return hitbox;
}

void Player::update(int deltaTime, const glm::vec2 levelPos, const float levelWidth, const float levelHeight)
{
	if (this->isDead()) return;

	// Update precedent position
	precPosition = position;
	precHitbox = hitbox;

	if (speed.x < 0) {
		this->left = true;
	}
	else {
		if (speed.x > 0) {
			this->left = false;
		}
	}
	
	// Update position
	position += 0.15f * speed * (float)deltaTime;

	// Update hitbox position and size
	hitbox.update(position + playerState[state].hitboxOffset, playerState[state].hitboxWidth, playerState[state].hitboxHeight);

	hitLevelBorder(levelPos, levelWidth, levelHeight);

	// Update speed
	//speed *= 0.99999999999 * (float)deltaTime;
	//if (glm::length(speed) < 0.1) speed = glm::vec2(0.0, 0.0);
	speed = glm::vec2(0.0f, 0.0f);

	// Update gun cooldown
	gunCooldown = max(0, gunCooldown - deltaTime);
}

bool Player::takeMushroom(Mushroom& mushroom)
{
	if (this->isDead()) return false;
	bool collision = hitbox.collision(mushroom.hitbox);

if (collision) {
	switch (mushroom.bonus) {
	case 1:
		this->evolve();
		break;
	case -1:
		this->degenerate();
		break;
	}
}

return collision;
}

bool Player::hitWall(const Wall& wall)
{
	bool collisionDetected = false;
	Hitbox hb = hitbox;

	glm::vec2 newPos(hb.position.x, precHitbox.position.y);

	glm::vec2 wallPos = wall.position;
	if (newPos.x + hb.width > wallPos.x
		&& newPos.y + hb.height > wallPos.y
		&& newPos.x < wallPos.x + wall.w
		&& newPos.y < wallPos.y + wall.h) {

		this->position = glm::vec2(precPosition.x, this->position.y);
		this->hitbox.position = glm::vec2(precHitbox.position.x, this->hitbox.position.y);
		collisionDetected = true;
	}

	newPos = glm::vec2(precHitbox.position.x, hb.position.y);
	if (newPos.x + hb.width > wallPos.x
		&& newPos.y + hb.height > wallPos.y
		&& newPos.x < wallPos.x + wall.w
		&& newPos.y < wallPos.y + wall.h) {

		this->position = glm::vec2(this->position.x, precPosition.y);
		this->hitbox.position = glm::vec2(this->hitbox.position.x, precHitbox.position.y);
		collisionDetected = true;
	}

	return collisionDetected;
}

void Player::hitLevelBorder(const glm::vec2 levelPos, const float levelWidth, const float levelHeight)
{
	Hitbox hb = hitbox;

	if (hb.position.x < levelPos.x && hb.position.x < precHitbox.position.x) {
		this->position = glm::vec2(precPosition.x, this->position.y);
		this->hitbox.position = glm::vec2(precHitbox.position.x, this->hitbox.position.y);
	}
	else {
		if (hb.position.x + hb.width > levelPos.x + levelWidth && hb.position.x > precHitbox.position.x) {
			this->position = glm::vec2(precPosition.x, this->position.y);
			this->hitbox.position = glm::vec2(precHitbox.position.x, this->hitbox.position.y);
		}
	}

	if (hb.position.y < levelPos.y && hb.position.y < precHitbox.position.y) {
		this->position = glm::vec2(this->position.x, precPosition.y);
		this->hitbox.position = glm::vec2(this->hitbox.position.x, precHitbox.position.y);
	}
	else {
		if (hb.position.y + hb.height > levelPos.y + levelHeight && hb.position.y > precHitbox.position.y) {
			this->position = glm::vec2(this->position.x, precPosition.y);
			this->hitbox.position = glm::vec2(this->hitbox.position.x, precHitbox.position.y);
		}
	}
}

Bullet Player::fire(glm::vec2 direction)
{
	PlayerState currentState = playerState[this->state];
	glm::vec2 modifiedDirection;

	if (direction.x == 0.0) {
		modifiedDirection = glm::vec2(this->speed.x, direction.y * currentState.bulletSpeed);
	}
	else {
		modifiedDirection = glm::vec2(direction.x * currentState.bulletSpeed, this->speed.y);
	}

	this->gunCooldown = currentState.gunCooldown;

	return Bullet(this->position.x + currentState.gunOffset.x,
		this->position.y + currentState.gunOffset.y, currentState.bulletRadius, modifiedDirection, this->weaponType, this->playerId);
}

void Player::draw(int time)
{
	PlayerState currentState = playerState[this->state];

	glm::vec2 screenPos = this->position + currentState.offset;

	//drawImage(this->texId[this->state], screenPos.x, screenPos.y, currentState.w, currentState.h, 1.0f);

	if (this->isDead()) {
		this->dead->draw(screenPos, 32.0 * currentState.zoom, 32.0 * currentState.zoom, time, this->left);
		return;
	}

	if (this->speed.x == 0 && this->speed.y == 0) {
		this->idle->draw(screenPos, 32.0 * currentState.zoom, 32.0 * currentState.zoom, time, this->left);
	}
	else {
		this->walk->draw(screenPos, 32.0 * currentState.zoom, 32.0 * currentState.zoom, time, this->left);
	}

	hitbox.draw();
}

void Player::toggleWeapon()
{
	weaponType = weaponType == REGRESS ? EVOLVE : REGRESS;
}

bool Player::isDead()
{
	return this->state == 0 || this->state == 6;
}

// class Bullet
Bullet::Bullet(float x, float y, float _radius, glm::vec2 _speed, BulletType _type, int _playerId)
	: position(glm::vec2(x, y)), radius(_radius), speed(_speed), type(_type), playerId(_playerId)
{
	stringstream ss;
	ss << playerId << (type == 0 ? "/bulletregress.png" : "/bulletevolve.png");
	this->texId = texturePool.getTexture(ss.str())->texId;
	this->hitbox = Hitbox(position, radius, radius, "bullet");
}

bool Bullet::update(int deltaTime, glm::vec2 levelPos, float levelWidth, float levelHeight, Player* players[4])
{
	// Update position
	position += 0.2f * speed * (float)deltaTime;

	// Update hitbox position and size
	hitbox.update(position, radius, radius);

	for (int i = 0; i < 4; i++) {
		if (players[i] == nullptr) continue;
		Hitbox playerHitbox = players[i]->getHitbox();
		if (players[i]->playerId != this->playerId && hitbox.collision(playerHitbox)) {
			players[i]->evolveOrRegress(type);
			return false;
		}
	}
	if (position.x < levelPos.x
		|| position.x > levelPos.x + levelWidth
		|| position.y < levelPos.y || position.y > levelPos.y + levelHeight) {
		return false; // bullet to delete
	}

	return true;
}

void Bullet::draw()
{
	drawImage(this->texId, position.x - radius, position.y - radius, this->radius * 2.0, this->radius * 2.0, 1.0f);
	hitbox.draw();
}

bool Bullet::hitWall(const Wall& wall)
{
	glm::vec2 wallPos = wall.position;
	if (position.x + radius > wallPos.x
		&& position.y + radius > wallPos.y
		&& position.x - radius < wallPos.x + wall.w
		&& position.y - radius < wallPos.y + wall.h) {

		return true;
	}

	return false;
}

/*const string levelString =
"...........\n"
".0...#...1.\n"
".....#.....\n"
"..v.##..^..\n"
"....##...#.\n"
".##.#v..##.\n"
".........#.\n"
"..^.....v..\n"
".....#.....\n"
".2...#...3.\n"
"...........";*/

const string levelString =
"................\n"
".0...#........1.\n"
"..........#.....\n"
"..v.##.......^..\n"
"....##........#.\n"
".##.#v.......##.\n"
"..............#.\n"
"..^..........v..\n"
".....#..........\n"
".....#..........\n"
".....#..........\n"
".....#..........\n"
".....#..........\n"
".....#..........\n"
".2...#........3.\n"
"................";

// class Level
Level::Level(const string level, float x, float y, float _w, float _h)
	: position(x, y), w(_w), h(_h)
{
	floor = texturePool.getTexture("floor.png")->texId;

	gridWidth = level.find('\n');
	gridHeight = (level.size() + 1) / (gridWidth + 1);

	tileHeight = w / (float)gridHeight;
	tileWidth = h / (float)gridWidth;
	cout << "Level dimentions: " << gridWidth << ", " << gridHeight << endl;

	int line = 0;
	int column = 0;
	for (int i = 0; i < level.size(); ++i) {
		if (level[i] != '\n') {
			column = i % (gridWidth + 1);
			line = i / (gridWidth + 1);
			this->initTile(level[i], column, line);
		}
	}

	// init wall texture coordinates
	for (int w = 0; w < walls.size(); ++w) {
		int neighbourCode = 0;
		for (int i = 0; i < walls.size(); ++i) {
			if (walls[i].column == walls[w].column) {
				if (walls[i].line == walls[w].line - 1) {
					neighbourCode += 1;
				}

				if (walls[i].line == walls[w].line + 1) {
					neighbourCode += 4;
				}
			}
			else if (walls[i].line == walls[w].line) {
				if (walls[i].column == walls[w].column - 1) {
					neighbourCode += 8;
				}

				if (walls[i].column == walls[w].column + 1) {
					neighbourCode += 2;
				}
			}
		}
		walls[w].initTexCoord(neighbourCode);
	}
}

Level::~Level() {}

string Level::getFilenameFromChar(char c)
{
	switch (c) {
	case '#':
		return "wall.png";
	default:
		return "floor.png";
	};
}

void Level::initTile(char c, int column, int line)
{
	glm::vec2 screenPos(glm::vec2((float)column * tileWidth, (float)line * tileHeight) + this->position);

	switch (c) {
	case '0':
		playerSpawns[0] = screenPos;
		break;
	case '1':
		playerSpawns[1] = screenPos;
		break;
	case '2':
		playerSpawns[2] = screenPos;
		break;
	case '3':
		playerSpawns[3] = screenPos;
		break;
	case '^':
		this->mushrooms.push_back(Mushroom(screenPos, tileWidth, tileHeight, true));
		break;
	case 'v':
		this->mushrooms.push_back(Mushroom(screenPos, tileWidth, tileHeight, false));
		break;
	case '#':
		this->walls.push_back(Wall(column, line, screenPos, tileWidth, tileHeight));
		break;
	};
}

void Level::draw(Player* players[4], int time)
{
	// TODO : si on veut pouvoir gérer la perspective il faudra pouvoir dessiner les objets du plus loin au plus proche en incluant les personnages

	drawImage(floor, position.x, position.y, w, h, 1.0f);

	for (int i = 0; i < mushrooms.size(); ++i) {
		mushrooms[i].draw();
	}

	for (int i = 0; i < walls.size(); ++i) {
		walls[i].draw();
	}

	for (int i = 0; i < 4; i++) {
		if (players[i] != nullptr) players[i]->draw(time);
	}

	for (int i = 0; i < bullets.size(); ++i) {
		bullets[i].draw();
	}
}

glm::vec2 Level::getPlayerSpawnScreenPosition(int playerIndex)
{
	return playerSpawns[playerIndex] * glm::vec2(tileWidth, tileHeight) + position;
}

void Level::update(int deltaTime, Player* players[4])
{
	for (int p = 0; p < 4; p++) {
		if (players[p] == nullptr) continue;

		for (int i = 0; i < mushrooms.size(); ++i) {
			if (players[p]->takeMushroom(mushrooms[i])) {
				mushrooms.erase(mushrooms.begin() + i);
				break;
			}
		}

		for (int i = 0; i < walls.size(); ++i) {
			players[p]->hitWall(walls[i]);
		}
	}

	for (int i = bullets.size() - 1; i >= 0; --i) {
		if (bullets[i].update(deltaTime, position, w, h, players) == false) {
			bullets.erase(bullets.begin() + i);
		}
	}

	for (int b = bullets.size() - 1; b >= 0; --b) {
		for (int i = 0; i < walls.size(); ++i) {
			if (bullets[b].hitWall(walls[i])) {
				bullets.erase(bullets.begin() + b);
				break;
			}
		}
	}


}

void Level::spawnBullet(Player* player, glm::vec2 direction) {
	if (!player->isDead() && player->gunCooldown == 0) {
		this->bullets.push_back(player->fire(direction));
	}
}
