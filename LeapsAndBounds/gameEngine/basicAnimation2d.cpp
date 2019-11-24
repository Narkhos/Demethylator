#include "basicAnimation2d.hpp"

GLfloat defaultTexCoord[64]
{
	0,0,
	0.25,0,
	0.25,1,
	0,1,

	0.25,0,
	0.5,0,
	0.5,1,
	0.25,1,

	0.5,0,
	0.75,0,
	0.75,1,
	0.5,1,

	0.75,0,
	1.0,0,
	1.0,1,
	0.75,1,

	0.25,0,
	0.0,0,
	0.0,1,
	0.25,1,

	0.5,0,
	0.25,0,
	0.25,1,
	0.5,1,

	0.75,0,
	0.5,0,
	0.5,1,
	0.75,1,

	1.0,0,
	0.75,0,
	0.75,1,
	1.0,1
};

// class BasicAnimation2d

BasicAnimation2d::BasicAnimation2d(const std::string& _file, float _speed, TexturePool& _texturePool)
	: speed(_speed), texCoord(defaultTexCoord)
{
	this->texId = _texturePool.getTexture(_file)->texId;
	cout << "texId: " << this->texId << endl;
}

BasicAnimation2d::~BasicAnimation2d() {}

void BasicAnimation2d::draw(glm::vec2 pos, float w, float h, int time, bool horizontalFlip, float alpha)
{
	GLfloat* tCoord = NULL;

	tCoord = texCoord
		+ (8 * ((time / (int)(this->speed)) % 4)
		+ (horizontalFlip ? 32 : 0));

	drawImage(texId, pos.x, pos.y, w, h, alpha, 1.0, tCoord);
}
