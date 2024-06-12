#pragma once
#include "glm-master/glm/glm.hpp"
#ifndef LIGHT_H
#define LIGHT_H

class Light
{
public:
	glm::vec3 color;
	glm::vec3 position;

	void setup(float r, float g, float b, float x, float y, float z);
};
#endif