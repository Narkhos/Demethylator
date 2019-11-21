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

bool showFPS = true;
TexturePool texturePool("./data/images/");

// GAME

// class Hitbox

Hitbox::Hitbox()
	:position(glm::vec2(0.0, 0.0)), radius(1.0), group("")
{
	this->texId = texturePool.getTexture("hitbox.png")->texId;
}

Hitbox::Hitbox(glm::vec2 _position, float _radius, string _group)
	: position(_position), radius(_radius), group(_group)
{
	this->texId = texturePool.getTexture("hitbox.png")->texId;
}

Hitbox::~Hitbox()
{

}

void Hitbox::update(glm::vec2 newPosition, float newRadius)
{
	this->position = newPosition;
	this->radius = newRadius;
}

bool Hitbox::collision(Hitbox& hitbox)
{
	return glm::distance(hitbox.position, this->position) < hitbox.radius + this->radius;
}

void Hitbox::draw()
{
	if (showFPS) drawImage(this->texId, this->position.x - radius, this->position.y - radius, this->radius * 2.0, this->radius * 2.0, 1.0f);
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
	this->hitbox = Hitbox(position + glm::vec2(w / 2.0, 2.0 * w / 3.0), w / 4.0, "bonus");
}

Mushroom::Mushroom(glm::vec2 _position, float _w, float _h, bool _bonus)
	:position(_position), w(_w), h(_h), bonus(_bonus ? 1 : -1)
{
	this->texId = texturePool.getTexture(_bonus ? "redmushroom.png" : "greenmushroom.png")->texId;
	this->hitbox = Hitbox(position + glm::vec2(w / 2.0, 2.0 * w / 3.0), w / 4.0, "bonus");
}

Mushroom::~Mushroom() {}

void Mushroom::draw()
{
	drawImage(this->texId, position.x, position.y, this->w, this->h, 1.0f);
	hitbox.draw();
}

PlayerState playerState[5] = {
	{
		glm::vec2(-3, -7),	// offset
		32.0f,				// w
		32.0f,				// h
		glm::vec2(15, 11),	// hitboxOffset
		8.0f,				// hitboxRadius
		1.00f,				// maxSpeed
		glm::vec2(15, 11),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-64, -64),	// offset
		128.0f,				// w
		128.0f,				// h
		glm::vec2(15, 11),	// hitboxOffset
		16.0f,				// hitboxRadius
		1.00f,				// maxSpeed
		glm::vec2(15, 11),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-64, -64),	// offset
		128.0f,				// w
		128.0f,				// h
		glm::vec2(15, 11),	// hitboxOffset
		16.0f,				// hitboxRadius
		1.00f,				// maxSpeed
		glm::vec2(15, 11),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-64, -64),	// offset
		128.0f,				// w
		128.0f,				// h
		glm::vec2(15, 11),	// hitboxOffset
		16.0f,				// hitboxRadius
		1.00f,				// maxSpeed
		glm::vec2(15, 11),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	},
	{
		glm::vec2(-64, -64),	// offset
		128.0f,				// w
		128.0f,				// h
		glm::vec2(15, 11),	// hitboxOffset
		16.0f,				// hitboxRadius
		1.00f,				// maxSpeed
		glm::vec2(15, 11),	// gunOffset
		8.0f,				// bulletRadius
		1.0f,				// bulletSpeed
		500					// gunCooldown
	}
};

GLfloat texCoord[64] =
{
	0, 0,
	0.25, 0,
	0.25, 1,
	0, 1,

	0.25, 0,
	0.5, 0,
	0.5, 1,
	0.25, 1,

	0.5, 0,
	0.75, 0,
	0.75, 1,
	0.5, 1,

	0.75, 0,
	1.0, 0,
	1.0, 1,
	0.75, 1,

	0.25, 0,
	0.0, 0,
	0.0, 1,
	0.25, 1,

	0.5, 0,
	0.25, 0,
	0.25, 1,
	0.5, 1,

	0.75, 0,
	0.5, 0,
	0.5, 1,
	0.75, 1,

	1.0, 0,
	0.75, 0,
	0.75, 1,
	1.0, 1
};

// class Player
Player::Player(float x, float y, int _state, BulletType _weaponType, int _playerId)
	:position(glm::vec2(x, y)), precPosition(glm::vec2(x, y)), state(_state), speed(glm::vec2(0.0, 0.0)), weaponType(_weaponType),
	gunCooldown(0), playerId(_playerId)
{
	for (int i = 0; i < 5; i++) {
		stringstream ss;
		ss << playerId << "/player" << i + 1 << ".png";
		this->texId[i] = texturePool.getTexture(ss.str())->texId;
	}

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
	this->hitbox = Hitbox(position + playerState[state].hitboxOffset, playerState[state].hitboxRadius, hitboxGroup.str());
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
	this->setState(min(this->state + 1, 4));
}

void Player::degenerate()
{
	this->setState(max(this->state - 1, 0));
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
	// Update precedent position
	precPosition = position;
	precHitbox = hitbox;

	// Update position
	position += 0.15f * speed * (float)deltaTime;

	// Update hitbox position and size
	hitbox.update(position + playerState[state].hitboxOffset, playerState[state].hitboxRadius);

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
	if (newPos.x + hb.radius > wallPos.x
		&& newPos.y + hb.radius > wallPos.y
		&& newPos.x - hb.radius < wallPos.x + wall.w
		&& newPos.y - hb.radius < wallPos.y + wall.h) {

		this->position = glm::vec2(precPosition.x, this->position.y);
		this->hitbox.position = glm::vec2(precHitbox.position.x, this->hitbox.position.y);
		collisionDetected = true;
	}

	newPos = glm::vec2(precHitbox.position.x, hb.position.y);
	if (newPos.x + hb.radius > wallPos.x
		&& newPos.y + hb.radius > wallPos.y
		&& newPos.x - hb.radius < wallPos.x + wall.w
		&& newPos.y - hb.radius < wallPos.y + wall.h) {

		this->position = glm::vec2(this->position.x, precPosition.y);
		this->hitbox.position = glm::vec2(this->hitbox.position.x, precHitbox.position.y);
		collisionDetected = true;
	}

	return collisionDetected;
}

void Player::hitLevelBorder(const glm::vec2 levelPos, const float levelWidth, const float levelHeight)
{
	Hitbox hb = hitbox;

	glm::vec2 newPos(hb.position.x, precHitbox.position.y);

	if (hb.position.x - hb.radius < levelPos.x || hb.position.x + hb.radius > levelPos.x + levelWidth) {
		this->position = glm::vec2(precPosition.x, this->position.y);
		this->hitbox.position = glm::vec2(precHitbox.position.x, this->hitbox.position.y);
	}

	newPos = glm::vec2(precHitbox.position.x, hb.position.y);

	if (hb.position.y - hb.radius < levelPos.y || hb.position.y + hb.radius> levelPos.y + levelHeight) {
		this->position = glm::vec2(this->position.x, precPosition.y);
		this->hitbox.position = glm::vec2(this->hitbox.position.x, precHitbox.position.y);
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

void Player::draw()
{
	PlayerState currentState = playerState[this->state];

	glm::vec2 screenPos = this->position + currentState.offset;

	drawImage(this->texId[this->state], screenPos.x, screenPos.y, currentState.w, currentState.h, 1.0f);

	hitbox.draw();
}

void Player::toggleWeapon()
{
	weaponType = weaponType == REGRESS ? EVOLVE : REGRESS;
}

// class Bullet
Bullet::Bullet(float x, float y, float _radius, glm::vec2 _speed, BulletType _type, int _playerId)
	: position(glm::vec2(x, y)), radius(_radius), speed(_speed), type(_type), playerId(_playerId)
{
	stringstream ss;
	ss << playerId << (type == 0 ? "/bulletregress.png" : "/bulletevolve.png");
	this->texId = texturePool.getTexture(ss.str())->texId;
	this->hitbox = Hitbox(position, radius, "bullet");
}

bool Bullet::update(int deltaTime, glm::vec2 levelPos, float levelWidth, float levelHeight, Player* players[4])
{
	// Update position
	position += 0.2f * speed * (float)deltaTime;

	// Update hitbox position and size
	hitbox.update(position, radius);

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

void Level::draw(Player* players[4])
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
		if (players[i] != nullptr) players[i]->draw();
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
	if (player->gunCooldown == 0) {
		this->bullets.push_back(player->fire(direction));
	}
}
