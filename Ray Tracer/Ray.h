#pragma once
#include "glm-master/glm/glm.hpp"
#include <iostream>
#ifndef RAY_H
#define RAY_H

using namespace glm;

class Ray
{
public:
	//p = u + v*t where v is the direction
	vec3 uWorld;
	vec3 vWorld;
	vec3 hitpointWorld;

	float tWorld;
	
	Ray(vec3 startPoint, vec3 direction);

	void calcHitPoint();

	float getT();

	float calcT(vec3 u, vec3 v);
};
#endif