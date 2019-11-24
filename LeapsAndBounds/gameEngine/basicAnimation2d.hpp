#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <glm/glm.hpp>
#include "gui.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

extern GLfloat defaultTexCoord[64];

class BasicAnimation2d
{
    public:
    GLuint texId;
    GLfloat *texCoord;

    float speed;

	BasicAnimation2d(const std::string& _file, float _speed, TexturePool& _texturePool);

	~BasicAnimation2d();

	void draw(glm::vec2 pos, float w, float h, int time, bool horizontalFlip = false, float alpha = 1.0);
};
