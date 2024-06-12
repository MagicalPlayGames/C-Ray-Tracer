#pragma once
#include "glm-master/glm/glm.hpp"
#include "ObjSeg.h"
#include "Material.h"
#ifndef OBJECT_H
#define OBJECT_H

class Object
{
public:
	ObjSeg* segments;
	int segCount = 0;
	vec3 Maxs = vec3(-FLT_MAX);
	vec3 Mins = vec3(FLT_MAX);
	Material* mats;
	int matCount = 0;

	void add(ObjSeg*);
	void add(Material);

	Material findMat(std::string);
	void updateSegments();
};
#endif
