#pragma once
#include "glm/glm.hpp"
#include <iostream>
#ifndef MATERIAL_H
#define MATERIAL_H

using namespace glm;

class Material
{
public:
	std::string name = "";
	float Ns;
	vec3 Ka = vec3(-1);
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	float Ni;
	float d;

	vec3* texture;
	int textureH;
	int textureW;
	bool textured = false;

	vec3* bump;
	int bH;
	int bW;
	bool bumped = false;

	bool reflective = false;

	void setup(std::string, float, vec3, vec3, vec3, vec3, float, float);
	void setTexture(vec3*,int,int);
	void setBump(vec3*, int, int);
};
#endif
