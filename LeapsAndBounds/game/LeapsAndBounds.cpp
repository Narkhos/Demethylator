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
#include "sfxr.hpp"
#include "gui.hpp"
#include "json.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

using namespace std;

SDL_Window* window;
TexturePool texturePool("./data/images/");

SDL_Color webColorToRGB(const char* fillStyle)
{
	SDL_Color rgb = { 255, 255, 255, 255 };
	string tmp(fillStyle);
	string r;
	string g;
	string b;

	if (strlen(fillStyle) == 4)
	{
		r = tmp.substr(1, 1) + tmp.substr(1, 1);
		g = tmp.substr(2, 1) + tmp.substr(2, 1);
		b = tmp.substr(3, 1) + tmp.substr(3, 1);
	}
	else
	{
		r = tmp.substr(1, 2);
		g = tmp.substr(3, 2);
		b = tmp.substr(5, 2);
	}

	stringstream r_ss;
	r_ss << std::hex << r;
	r_ss >> rgb.r;

	stringstream g_ss;
	g_ss << std::hex << g;
	g_ss >> rgb.g;

	stringstream b_ss;
	b_ss << std::hex << b;
	b_ss >> rgb.b;

	// cout << fillStyle << " : " << r << g << b << " : " << rgb.r << ", "<< rgb.g << ", "<< rgb.b <<endl;
	return rgb;
}

class DiscreetGauge
{
public:
	vector<GUI_Button> buttonList;
	TTF_Font* font;
	GLuint labelTexId;
	float x;
	float y;
	int value;
	wstring label;
	int label_w;
	int label_h;
	SDL_Color textColor;

	DiscreetGauge(
		int initValue,
		int textureButton,
		int textureButtonZero,
		float _x,
		float _y,
		float buttonWidth,
		float buttonHeight,
		int length,
		TTF_Font* _font,
		wstring _label,
		SDL_Color _textColor)
		:value(initValue), x(_x), y(_y), font(_font), textColor(_textColor), label(_label)
	{
		labelTexId = texteToTexture(font, textColor, _label, this->label_w, this->label_h);

		int idTextureButton = textureButtonZero;
		for (int i = 0; i < length; i++)
		{
			buttonList.push_back(GUI_Button(idTextureButton, _x + (i * buttonWidth), _y + this->label_h, buttonWidth, buttonHeight, 2));
			idTextureButton = textureButton;
		}

		this->updateButtons();
	}

	void updateTextColor(const SDL_Color &newColor)
	{
		if (newColor.r != this->textColor.r
			|| newColor.g != this->textColor.g
			|| newColor.b != this->textColor.b
			|| newColor.a != this->textColor.a)
		{
			this->textColor = newColor;
			glDeleteTextures(1, &labelTexId);
			labelTexId = texteToTexture(font, this->textColor, this->label, this->label_w, this->label_h);
		}
	}

	void updateButtons()
	{
		// Change state of each button in the gauge
		for (int i = 0; i < this->buttonList.size(); i++)
		{
			buttonList[i].setEtat(i <= this->value ? 0 : 1);
		}
	}

	int onClick(int mouse_x, int mouse_y)
	{
		int newIndex = -1;
		for (int i = 0; i < this->buttonList.size(); i++)
		{
			if (this->buttonList[i].isIn(mouse_x, mouse_y))
			{
				newIndex = i;
				break;
			}
		}

		if (newIndex == -1)
		{
			return -1; // The user does not clicked this gauge
		}

		this->value = newIndex;

		this->updateButtons();

		return this->value; // return gauge value (changed or not)
	}

	void draw()
	{
		drawImage(labelTexId, x, y, (float)this->label_w, (float)this->label_h, 1.0f);
		for (int i = 0; i < this->buttonList.size(); i++)
		{
			buttonList[i].draw();
		}
	}

};

class Config
{
public:
	string filepath;
	nlohmann::json data;

	Config(string _filepath)
		:filepath(_filepath)
	{
		ifstream i(this->filepath);

		i >> data;
	}

	void save()
	{
		ofstream o(filepath);
		o << setw(4) << data << endl;
	}

	void trace()
	{
		cout << data << endl;
	}
};

TTF_Font* font = nullptr;

TTF_Font* loadFont(string filepath)
{
	// INITIALISATION de SDL_ttf
	if (!TTF_WasInit() && TTF_Init() == -1)
	{
		cerr << "TTF_Init : " << TTF_GetError() << endl;
		return NULL;
	}

	TTF_Font* font = TTF_OpenFont(filepath.c_str(), 16);

	if (!font)
	{
		cerr << "TTF_OpenFont : " << TTF_GetError() << endl;
		return NULL;
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	return font;
}

int width = 1024;
int height = 768;
int windowedWidth = 1024;
int windowedHeight = 768;
bool fullscreen = false;
bool showFPS = false;

string gameTitle = "Leaps and Bounds";

void updateAndDrawFPS(int currentTime)
{
	static int frame = 0;
	static GLuint texId = 0;
	static int w = 0;
	static int h = 0;

	frame++;

	static int prec_time = 0;

	int deltatime = currentTime - prec_time;

	if (deltatime >= 1000)
	{
		SDL_Color couleur = { 255, 255, 255, 255 };

		if (texId > 0) glDeleteTextures(1, &texId);
		wstringstream ss;
		ss << L"FPS: " << frame;
		texId = texteToTexture(font, couleur, ss.str(), w, h);
		prec_time = currentTime;
		frame = 0;
	}

	// Draw FPS texture
	if (texId > 0)
	{
		drawImage(texId, 4, 0, (float)w, (float)h, 1.0f);
	}

}

// CONTROLLERS MANAGEMENT

map<int, SDL_GameController *> controllers; ///< Liste des controllers branchés

int addController(int id)
{
	if (SDL_IsGameController(id)) {
		SDL_GameController *pad = SDL_GameControllerOpen(id);

		if (pad)
		{
			SDL_Joystick *joy = SDL_GameControllerGetJoystick(pad);
			int instanceID = SDL_JoystickInstanceID(joy);

			controllers[instanceID] = pad;
			cout << "Branchement du controller " << instanceID << endl;
			return instanceID;
		}
	}
	return -1;
}

void removeController(int id)
{
	cout << "Debranchement du controller " << id << endl;
	SDL_GameController *pad = controllers[id];
	SDL_GameControllerClose(pad);
	controllers[id] = NULL;
	controllers.erase(id);
}

void foundControllers()
{
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		addController(i);
	}
}

void windowResize(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	glLoadIdentity();

	glOrtho(0, width, height, 0, -1, 1);
}

void toggleFullscreen()
{
	if (fullscreen)
	{
		SDL_SetWindowFullscreen(window, 0);
		cout << width << ", " << height << endl;
		windowResize(windowedWidth, windowedHeight);
		fullscreen = false;
	}
	else
	{
		windowedWidth = width;
		windowedHeight = height;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		fullscreen = true;
	}
}


// GAME

class Hitbox {
public:
	glm::vec2 position;
	float radius;
	GLuint texId;
	string group;
	Hitbox()
		:position(glm::vec2(0.0, 0.0)), radius(1.0), group("")
	{}

	Hitbox(glm::vec2 _position, float _radius, string _group)
		:position(_position), radius(_radius), group(_group)
	{
		this->texId = texturePool.getTexture("hitbox.png")->texId;
	}
	~Hitbox()
	{

	}

	void update(glm::vec2 newPosition, float newRadius)
	{
		this->position = newPosition;
		this->radius = newRadius;
	}

	bool collision(Hitbox& hitbox)
	{
		return glm::distance(hitbox.position, this->position) < hitbox.radius + this->radius;
	}

	void draw()
	{
		if (showFPS) drawImage(this->texId, this->position.x - radius, this->position.y - radius, this->radius * 2.0, this->radius * 2.0, 1.0f);
	}
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

	Wall(int _column, int _line, float x, float y, float _w, float _h)
		:column(_column), line(_line), position(x, y), w(_w), h(_h)
	{
		this->texId = texturePool.getTexture("wall.png")->texId;
		initTexCoord(0);
	}

	Wall(int _column, int _line, glm::vec2 _position, float _w, float _h)
		:column(_column), line(_line), position(_position), w(_w), h(_h)
	{
		this->texId = texturePool.getTexture("wall.png")->texId;
		initTexCoord(0);
	}

	~Wall() {}

	void initTexCoord(int neighbourCode)
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

	void draw()
	{
		drawImage(this->texId, position.x, position.y, this->w, this->h, 1.0f, 1.0f, texCoord);
	}
};

class Mushroom {
public:
	int bonus;
	GLuint texId;
	glm::vec2 position;
	float w;
	float h;
	Hitbox hitbox;

	Mushroom(float x, float y, float _w, float _h, bool _bonus)
		:position(x, y), w(_w), h(_h), bonus(_bonus ? 1 : -1)
	{
		this->texId = texturePool.getTexture(_bonus ? "redmushroom.png" : "greenmushroom.png")->texId;
		this->hitbox = Hitbox(position + glm::vec2(w / 2.0, 2.0 * w / 3.0), w / 4.0, "bonus");
	}

	Mushroom(glm::vec2 _position, float _w, float _h, bool _bonus)
		:position(_position), w(_w), h(_h), bonus(_bonus ? 1 : -1)
	{
		this->texId = texturePool.getTexture(_bonus ? "redmushroom.png" : "greenmushroom.png")->texId;
		this->hitbox = Hitbox(position + glm::vec2(w / 2.0, 2.0 * w / 3.0), w / 4.0, "bonus");
	}

	~Mushroom() {}

	void draw()
	{
		drawImage(this->texId, position.x, position.y, this->w, this->h, 1.0f);
		hitbox.draw();
	}
};

enum BulletType { REGRESS = 0, EVOLVE = 1};

class Bullet {
public:
	GLuint texId;
	Hitbox hitbox;
	glm::vec2 position;
	float radius;
	int type; // 0: REGRESS, 1: EVOLVE

	glm::vec2 speed;

	Bullet(float x, float y, float _radius, glm::vec2 _speed, BulletType _type)
		: position(glm::vec2(x, y)), radius(_radius), speed(_speed), type(_type)
	{
		this->texId = texturePool.getTexture(type == 0 ? "bulletregress.png" : "bulletevolve.png")->texId;
		this->hitbox = Hitbox(position, radius, "bullet");
	}

	bool update(int deltaTime, glm::vec2 levelPos, float levelWidth, float levelHeight)
	{
		// Update position
		position += 0.2f * speed * (float)deltaTime;

		// Update hitbox position and size
		hitbox.update(position, radius);

		if (position.x < levelPos.x
			|| position.x > levelPos.x + levelWidth
			|| position.y < levelPos.y || position.y > levelPos.y + levelHeight) {
			return false; // bullet to delete
		}

		return true;
	}

	void draw()
	{
		drawImage(this->texId, position.x - radius, position.y - radius, this->radius * 2.0, this->radius * 2.0, 1.0f);
		hitbox.draw();
	}

	bool hitWall(const Wall& wall)
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
};

class Player {
public:
	int level;
	GLuint texId[5];
	Hitbox hitbox[5];
	Hitbox precHitbox;
	glm::vec2 position;
	glm::vec2 precPosition;
	glm::vec2 gunLocalPos;
	int gunCooldown;
	float w;
	float h;
	glm::vec2 speed;
	BulletType weaponType;

	Player(float x, float y, float _w, float _h, int _level, BulletType _weaponType)
		:position(glm::vec2(x, y)), precPosition(glm::vec2(x, y)), w(_w), h(_h), level(_level), speed(glm::vec2(0.0, 0.0)), weaponType(_weaponType),
		gunCooldown(0)
	{
		for (int i = 0; i < 5; i++) {
			stringstream ss;
			ss << "player" << i + 1 << ".png";
			this->texId[i] = texturePool.getTexture(ss.str())->texId;
			this->hitbox[i] = Hitbox(position + glm::vec2(w / 2.0, w - 1.0 / 8.0 * w), w / 8.0, "player0");
			this->gunLocalPos = glm::vec2(w / 2.0, w - 1.0 / 8.0 * w);
		}

		precHitbox = this->hitbox[level];
	}

	~Player()
	{
	}

	void evolve()
	{
		this->level = min(this->level + 1, 4);
	}

	void degenerate()
	{
		this->level = max(this->level - 1, 0);
	}

	void update(int deltaTime, const glm::vec2 levelPos, const float levelWidth, const float levelHeight)
	{
		// Update precedent position
		precPosition = position;
		precHitbox = hitbox[this->level];

		// Update position
		position += 0.15f * speed * (float)deltaTime;

		// Update hitbox position and size
		hitbox[this->level].update(position + glm::vec2(w / 2.0, w - 1.0 / 8.0 * w), w / 8.0);

		hitLevelBorder(levelPos, levelWidth, levelHeight);

		// Update speed
		//speed *= 0.99999999999 * (float)deltaTime;
		//if (glm::length(speed) < 0.1) speed = glm::vec2(0.0, 0.0);
		speed = glm::vec2(0.0f, 0.0f);

		// Update gun cooldown
		gunCooldown = max(0, gunCooldown - deltaTime);
	}

	bool takeMushroom(Mushroom& mushroom)
	{
		bool collision = hitbox[level].collision(mushroom.hitbox);

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

	bool hitWall(const Wall& wall)
	{
		bool collisionDetected = false;
		Hitbox hb = hitbox[level];

		glm::vec2 newPos(hb.position.x, precHitbox.position.y);

		glm::vec2 wallPos = wall.position;
		if (newPos.x + hb.radius > wallPos.x
			&& newPos.y + hb.radius > wallPos.y
			&& newPos.x - hb.radius < wallPos.x + wall.w
			&& newPos.y - hb.radius < wallPos.y + wall.h) {

			this->position = glm::vec2(precPosition.x, this->position.y);
			this->hitbox[level].position = glm::vec2(precHitbox.position.x, this->hitbox[level].position.y);
			collisionDetected = true;
		}

		newPos = glm::vec2(precHitbox.position.x, hb.position.y);
		if (newPos.x + hb.radius > wallPos.x
			&& newPos.y + hb.radius > wallPos.y
			&& newPos.x - hb.radius < wallPos.x + wall.w
			&& newPos.y - hb.radius < wallPos.y + wall.h) {

			this->position = glm::vec2(this->position.x, precPosition.y);
			this->hitbox[level].position = glm::vec2(this->hitbox[level].position.x, precHitbox.position.y);
			collisionDetected = true;
		}

		return collisionDetected;
	}

	void hitLevelBorder(const glm::vec2 levelPos, const float levelWidth, const float levelHeight)
	{
		Hitbox hb = hitbox[level];

		glm::vec2 newPos(hb.position.x, precHitbox.position.y);

		if (hb.position.x - hb.radius < levelPos.x || hb.position.x + hb.radius > levelPos.x + levelWidth) {
			this->position = glm::vec2(precPosition.x, this->position.y);
			this->hitbox[level].position = glm::vec2(precHitbox.position.x, this->hitbox[level].position.y);
		}

		newPos = glm::vec2(precHitbox.position.x, hb.position.y);

		if (hb.position.y - hb.radius < levelPos.y || hb.position.y + hb.radius> levelPos.y + levelHeight) {
			this->position = glm::vec2(this->position.x, precPosition.y);
			this->hitbox[level].position = glm::vec2(this->hitbox[level].position.x, precHitbox.position.y);
		}
	}

	void draw()
	{
		drawImage(this->texId[this->level], position.x, position.y, this->w, this->h, 1.0f);
		hitbox[this->level].draw();
	}

	void toggleWeapon()
	{
		weaponType = weaponType == REGRESS ? EVOLVE : REGRESS;
	}
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

	Level(const string level, float x, float y, float _w, float _h)
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
				} else if (walls[i].line == walls[w].line) {
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

	~Level() {}

	string getFilenameFromChar(char c)
	{
		switch (c) {
		case '#':
			return "wall.png";
		default:
			return "floor.png";
		};
	}

	void initTile(char c, int column, int line)
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

	void draw(Player& player)
	{
		// TODO : si on veut pouvoir gérer la perspective il faudra pouvoir dessiner les objets du plus loin au plus proche en incluant les personnages

		drawImage(floor, position.x, position.y, w, h, 1.0f);

		for (int i = 0; i < mushrooms.size(); ++i) {
			mushrooms[i].draw();
		}

		for (int i = 0; i < walls.size(); ++i) {
			walls[i].draw();
		}

		player.draw();

		for (int i = 0; i < bullets.size(); ++i) {
			bullets[i].draw();
		}
	}

	glm::vec2 getPlayerSpawnScreenPosition(int playerIndex)
	{
		return playerSpawns[playerIndex] * glm::vec2(tileWidth, tileHeight) + position;
	}

	void update(int deltaTime, Player& player)
	{
		for (int i = 0; i < mushrooms.size(); ++i) {
			if (player.takeMushroom(mushrooms[i])) {
				mushrooms.erase(mushrooms.begin() + i);
				break;
			}
		}

		for (int i = 0; i < walls.size(); ++i) {
			player.hitWall(walls[i]);
		}

		for (int i = bullets.size() - 1; i >= 0; --i) {
			if (bullets[i].update(deltaTime, position, w, h) == false) {
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

	void spawnBullet(Player& player, glm::vec2 direction) {
		glm::vec2 modifiedDirection;

		if (direction.x == 0.0) {
			modifiedDirection = glm::vec2(player.speed.x, direction.y);
		}
		else {
			modifiedDirection = glm::vec2(direction.x, player.speed.y);
		}
		if (player.gunCooldown == 0) {
			this->bullets.push_back(
				Bullet(player.position.x + player.gunLocalPos.x, player.position.y + player.gunLocalPos.y, 8, modifiedDirection, player.weaponType));
			player.gunCooldown = 400;
		}
	}
};

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

	foundControllers();

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

	// initSfxr();

	font = loadFont("./data/fonts/OxygenMono-Regular.ttf");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0);
	glViewport(0, 0, width, height);
	glOrtho(0, width, height, 0, -1, 1);

	// Disable vsync for performance purpose
	SDL_GL_SetSwapInterval(0);

	glewInit();

	Level level(levelString, 192, 64, 640, 640);
	Player player1(level.playerSpawns[0].x, level.playerSpawns[0].y, 128, 128, 2, REGRESS);
	GLuint background = texturePool.getTexture("background.png")->texId;

	// INIT MAIN LOOP
	bool running = true;
	int currentTime = SDL_GetTicks();
	int precTime = currentTime;
	int deltaTime = 0;

	int mouse_x = 0;
	int mouse_y = 0;

	// MAIN LOOP
	while (running)
	{
		// TIME
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - precTime;
		precTime = currentTime;

		// UPDATES
		player1.update(deltaTime, level.position, level.w, level.h);
		level.update(deltaTime, player1);
		// MOUSE
		SDL_GetMouseState(&mouse_x, &mouse_y);

		// INPUTS
		SDL_Keymod keyMod = SDL_GetModState();

		const Uint8* keyState = SDL_GetKeyboardState(nullptr);

		if (keyState[SDL_SCANCODE_W]) {
			player1.speed.y = -1;
		}

		if (keyState[SDL_SCANCODE_S]) {
			player1.speed.y = 1;
		}

		if (keyState[SDL_SCANCODE_A]) {
			player1.speed.x = -1;
		}

		if (keyState[SDL_SCANCODE_D]) {
			player1.speed.x = 1;
		}

		if (keyState[SDL_SCANCODE_UP]) {
			level.spawnBullet(player1, glm::vec2(0.0, -1.0));
		}

		if (keyState[SDL_SCANCODE_DOWN]) {
			level.spawnBullet(player1, glm::vec2(0.0, 1.0));
		}

		if (keyState[SDL_SCANCODE_LEFT]) {
			level.spawnBullet(player1, glm::vec2(-1.0, 0.0));
		}

		if (keyState[SDL_SCANCODE_RIGHT]) {
			level.spawnBullet(player1, glm::vec2(1.0, 0.0));
		}

		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_CONTROLLERDEVICEADDED:
				addController(event.cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				removeController(event.cdevice.which);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				switch (event.cbutton.button)
				{
				case SDL_CONTROLLER_BUTTON_START:

					break;
				case SDL_CONTROLLER_BUTTON_A:

					break;
				case SDL_CONTROLLER_BUTTON_B:

					break;
				case SDL_CONTROLLER_BUTTON_BACK:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:

					break;
				default:
					cout << (int)(event.cbutton.button) << endl;
				}
				break;
			case SDL_CONTROLLERBUTTONUP:
				switch (event.cbutton.button)
				{
				case SDL_CONTROLLER_BUTTON_START:

					break;
				case SDL_CONTROLLER_BUTTON_A:

					break;
				case SDL_CONTROLLER_BUTTON_B:

					break;
				case SDL_CONTROLLER_BUTTON_BACK:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP:

					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:

					break;
				default:
					cout << (int)(event.cbutton.button) << endl;
				}
				break;
			case SDL_MOUSEMOTION:

				break;
			case SDL_MOUSEBUTTONDOWN:

				break;
			case SDL_MOUSEBUTTONUP:

				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					windowResize(event.window.data1, event.window.data2);
				}
				break;
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					if (keyMod & KMOD_ALT)
					{
						toggleFullscreen();
					}
				}

				if (event.key.keysym.sym == SDLK_TAB)
				{
					showFPS = !showFPS;
				}

				if (event.key.keysym.sym == SDLK_SPACE)
				{
					player1.toggleWeapon();
				}

				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_p) {
					player1.evolve();
				}

				if (event.key.keysym.sym == SDLK_o) {
					player1.degenerate();
				}
				break;
			}
		}

		// DISPLAY
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawImage(background, 0, 0, 1024, 768, 1.0f);
		// display framerate
		if (showFPS) updateAndDrawFPS(currentTime);

		level.draw(player1);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glContext);
	glContext = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}