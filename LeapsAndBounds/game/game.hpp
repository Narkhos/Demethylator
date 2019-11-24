#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <glm/glm.hpp>
#include "gui.hpp"
#include "basicAnimation2d.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

using namespace std;

extern bool showFPS;
extern TexturePool texturePool;

// GAME

class Bullet;

class Hitbox {
public:
	glm::vec2 position;
	float width;
	float height;
	GLuint texId;
	string group;

	Hitbox();

	Hitbox(glm::vec2 _position, float _width, float _height, string _group);
	~Hitbox();

	void update(glm::vec2 newPosition, float newWidth, float newHeight);

	bool collision(Hitbox& hitbox);

	void draw();
};

class Wall {
public:
	GLuint texId;
	glm::vec2 position;
	float w;
	float h;
	GLfloat texCoord[8];
	int column;
	int line;

	Wall(int _column, int _line, float x, float y, float _w, float _h);

	Wall(int _column, int _line, glm::vec2 _position, float _w, float _h);

	~Wall();

	void initTexCoord(int neighbourCode);

	void draw();
};

class Mushroom {
public:
	int bonus;
	GLuint texId;
	glm::vec2 position;
	float w;
	float h;
	Hitbox hitbox;

	Mushroom(float x, float y, float _w, float _h, bool _bonus);

	Mushroom(glm::vec2 _position, float _w, float _h, bool _bonus);

	~Mushroom();

	void draw();
};

enum BulletType { REGRESS = 0, EVOLVE = 1 };


struct PlayerState {
	glm::vec2 offset;
	float zoom;
	glm::vec2 hitboxOffset;
	float hitboxWidth;
	float hitboxHeight;
	float maxSpeed;
	glm::vec2 gunOffset;
	float bulletRadius;
	float bulletSpeed;
	int gunCooldown;
};

extern PlayerState playerState[7];

class Player {
public:
	int state;
	Hitbox hitbox;
	Hitbox precHitbox;
	glm::vec2 position;
	glm::vec2 precPosition;
	int gunCooldown;
	glm::vec2 speed;
	BulletType weaponType;
	int playerId;

	BasicAnimation2d* walk;
	BasicAnimation2d* idle;
	BasicAnimation2d* dead;
	bool left;
	float zoom;

	Player(float x, float y, int _state, BulletType _weaponType, int _playerId);

	~Player();

	void setState(int _state);

	void setSpeedX(float value);

	void setSpeedY(float value);

	void evolve();

	void degenerate();

	void evolveOrRegress(BulletType type);

	Hitbox getHitbox();

	void update(int deltaTime, const glm::vec2 levelPos, const float levelWidth, const float levelHeight);

	bool takeMushroom(Mushroom& mushroom);

	bool hitWall(const Wall& wall);

	void hitLevelBorder(const glm::vec2 levelPos, const float levelWidth, const float levelHeight);

	Bullet fire(glm::vec2 direction);

	void draw(int time);

	void toggleWeapon();

	bool isDead();
};

class Bullet {
public:
	GLuint texId;
	Hitbox hitbox;
	glm::vec2 position;
	float radius;
	BulletType type; // 0: REGRESS, 1: EVOLVE
	int playerId;

	glm::vec2 speed;

	Bullet(float x, float y, float _radius, glm::vec2 _speed, BulletType _type, int _playerId);

	bool update(int deltaTime, glm::vec2 levelPos, float levelWidth, float levelHeight, Player* players[4]);

	void draw();

	bool hitWall(const Wall& wall);
};

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

extern const string levelString;

class Level {
public:
	int gridWidth;
	int gridHeight;
	glm::vec2 position;
	float w;
	float h;
	float tileWidth;
	float tileHeight;
	glm::vec2 playerSpawns[4];
	vector<Wall> walls;
	GLuint floor;

	vector<Mushroom> mushrooms;
	vector<Bullet> bullets;

	Level(const string level, float x, float y, float _w, float _h);

	~Level();

	string getFilenameFromChar(char c);

	void initTile(char c, int column, int line);

	void draw(Player* players[4], int time);

	glm::vec2 getPlayerSpawnScreenPosition(int playerIndex);

	void update(int deltaTime, Player* players[4]);

	void spawnBullet(Player* player, glm::vec2 direction);
};
