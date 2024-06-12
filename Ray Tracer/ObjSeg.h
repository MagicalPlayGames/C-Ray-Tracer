#pragma once
#include "glm-master/glm/glm.hpp"
#include "Face.h"
#include "Vertex.h"
#ifndef ObjSeg_H
#define ObjSeg_H

class ObjSeg
{
public:
	Vertex* verticies;
	Face* faces;
	ObjSeg* nextSeg = nullptr;
	vec3* normals;
	vec2* texPos;
	int numOfVs = 0;
	int numOfFs = 0;
	int numOfNs = 0;
	int numOfTPs = 0;
	vec3 Maxs = vec3(-FLT_MAX);
	vec3 Mins = vec3(FLT_MAX);

	void addVertex(Vertex v);
	void addFace(Vertex* v);
	void addNormal(vec3 v);
	void addtexPos(vec2 v);
	bool addSegment(ObjSeg*);
	void updateSegment();
	void splitFaces(int, ObjSeg*);

	void copy(ObjSeg*);
};
#endif

