#include "Texture.h"

using namespace glm;

void Texture::setup(vec3* tex, int width, int height,std::string n)
{
	texture = tex;
	textureW = width;
	textureH = height;
	name = n;
}