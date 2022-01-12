#pragma once
#include "glm/glm.hpp"
#include "Vertex.h"
#include <iostream>
#ifndef FACE_H
#define FACE_H

using namespace glm;

class Face
{
public:
	int objIndex = 0;
	int objSegIndex = 0;
	Vertex* verticies;
	vec3 fNormal = vec3(-1.0f);
	float curT= FLT_MAX;
	vec2 Tmins = vec2(1);
	vec2 Tmaxs = vec2(-1);
	vec2 Pmins = vec2(1);
	vec2 Pmaxs = vec2(-1);
	std::string matName = "";

	void setup(Vertex* v);

	void setTree(int, int);

	void setMat(std::string f);
};
#endif
