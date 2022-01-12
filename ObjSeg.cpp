#include "ObjSeg.h"
#include <iostream>
using namespace glm;


void ObjSeg::addVertex(Vertex v)
{
	if (numOfVs > 0)
	{
		Vertex* temp = new Vertex[numOfVs];
		for (int i = 0; i < numOfVs; i++)
		{
			temp[i] = verticies[i];
		}
		delete[] verticies;
		verticies = new Vertex[numOfVs + 1];
		for (int i = 0; i < numOfVs; i++)
		{
			verticies[i] = temp[i];
		}
		delete[] temp;
	}
	else
	{
		verticies = new Vertex[1];
	}
	verticies[numOfVs] = v;
	numOfVs++;
}

void ObjSeg::addFace(Vertex* v)
{
	if (numOfFs > 0)
	{
		Face* temp = faces;
		faces = new Face[numOfFs + 1];
		for (int i = 0; i < numOfFs; i++)
		{
			faces[i] = temp[i];
		}
		delete[] temp;
	}
	else
	{
		faces = new Face[1];
	}
	Face addedFace;
	addedFace.setup(v);
	for (int i = 0; i < 3; i++)
	{
		vec3 Vpos = addedFace.verticies[i].position;
		if (Maxs.x < Vpos.x)
			Maxs.x = Vpos.x;
		if (Mins.x > Vpos.x)
			Mins.x = Vpos.x;

		if (Maxs.y < Vpos.y)
			Maxs.y = Vpos.y;
		if (Mins.y > Vpos.y)
			Mins.y = Vpos.y;

		if (Maxs.z < Vpos.z)
			Maxs.z = Vpos.z;
		if (Mins.z > Vpos.z)
			Mins.z = Vpos.z;
	}
	faces[numOfFs] = addedFace;
	numOfFs++;
}

void ObjSeg::addNormal(vec3 v)
{
	if (numOfNs > 0)
	{
		vec3* temp = new vec3[numOfNs];
		for (int i = 0; i < numOfNs; i++)
		{
			temp[i] = normals[i];
		}
		delete[] normals;
		normals = new vec3[numOfNs + 1];
		for (int i = 0; i < numOfNs; i++)
		{
			normals[i] = temp[i];
		}
		delete[] temp;
	}
	else
	{
		normals = new vec3[1];
	}
	normals[numOfNs] = v;
	numOfNs++;
}

void ObjSeg::addtexPos(vec2 v)
{
	if (numOfTPs > 0)
	{
		vec2* temp = new vec2[numOfTPs];
		for (int i = 0; i < numOfTPs; i++)
		{
			temp[i] = texPos[i];
		}
		delete[] texPos;
		texPos = new vec2[numOfTPs + 1];
		for (int i = 0; i < numOfTPs; i++)
		{
			texPos[i] = temp[i];
		}
		delete[] temp;
	}
	else
	{
		texPos = new vec2[1];
	}
	texPos[numOfTPs] = v;
	numOfTPs++;
}

bool ObjSeg::addSegment(ObjSeg* seg)
{
	bool added = false;
	if (nextSeg==nullptr)
	{
		nextSeg = seg;
		added = true;
	}
	return added;
}

void ObjSeg::splitFaces(int i,ObjSeg* top)
{
	int start = i;
	if (i != -1)
	{
		for (;start < i + 20 && start<top->numOfFs; start++)
		{
			this->addFace(top->faces[start].verticies);
		}
	}
	else
	{
		start = 0;
	}
	if (start < top->numOfFs)
	{
		this->nextSeg = new ObjSeg();
		this->nextSeg->splitFaces(start, top);
	}
	
}


void ObjSeg::updateSegment()
{
	if (nextSeg != nullptr)
	{
		nextSeg->updateSegment();

		vec3 Vpos = nextSeg->Maxs;

		if (Maxs.x < Vpos.x)
			Maxs.x = Vpos.x;
		if (Maxs.y < Vpos.y)
			Maxs.y = Vpos.y;
		if (Maxs.z < Vpos.z)
			Maxs.z = Vpos.z;

		Vpos = nextSeg->Mins;

		if (Mins.x > Vpos.x)
			Mins.x = Vpos.x;
		if (Mins.y > Vpos.y)
			Mins.y = Vpos.y;
		if (Mins.z > Vpos.z)
			Mins.z = Vpos.z;
	}
}

void ObjSeg::copy(ObjSeg* toCopy)
{
	Mins = toCopy->Mins;
	Maxs = toCopy->Maxs;
	faces = toCopy->faces;
	verticies = toCopy->verticies;
	normals = toCopy->normals;
	texPos = toCopy->texPos;
	nextSeg = toCopy->nextSeg;
	numOfFs = toCopy->numOfFs;
	numOfNs = toCopy->numOfNs;
	numOfTPs = toCopy->numOfTPs;
	numOfVs = toCopy->numOfVs;

}