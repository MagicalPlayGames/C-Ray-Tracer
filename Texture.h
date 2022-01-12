#pragma once
#include "glm/glm.hpp"
#include <iostream>
#ifndef Texture_H
#define Texture_H

using namespace glm;

class Texture
{
public:
	vec3* texture;
	int textureH = 0;
	int textureW = 0;
	std::string name;

	void setup(vec3*, int, int, std::string);
};
#endif
