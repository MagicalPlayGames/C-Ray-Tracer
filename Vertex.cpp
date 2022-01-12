#include "Vertex.h"
using namespace glm;

void Vertex::addPosition(vec3 v)
{
	position = v;
}

void Vertex::changePosition(mat4 O2W)
{
	position = O2W*vec4(position, 1.0f);
}

void Vertex::setup(vec3 pos, vec3 norm, vec2 tPos)
{
	position = pos;
	normal = norm;
	texturePosition = tPos;
}