#pragma once
#include "glm-master/glm/glm.hpp"
#ifndef VERTEX_H
#define VERTEX_H

using namespace glm;

class Vertex
{
public:
	vec3 position;
	vec3 normal;
	vec2 texturePosition = vec2(-1);

	void addPosition(vec3 v);

	void changePosition(mat4 O2W);

	void setup(vec3 pos,vec3 norm,vec2 tPos);
};
#endif
