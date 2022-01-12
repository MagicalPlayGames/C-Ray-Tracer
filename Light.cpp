#include "Light.h"
using namespace glm;


	void Light::setup(float r, float g, float b, float x, float y, float z)
	{
		color = vec3(r, g, b);
		position = vec3(x, y, z);
	}