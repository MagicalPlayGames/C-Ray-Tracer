#include "Object.h"
using namespace glm;

void Object::add(ObjSeg* o)
{

	if (segCount > 0)
	{
		ObjSeg* temp = new ObjSeg[segCount];
		for (int i = 0; i < segCount; i++)
		{
			temp[i] = segments[i];
		}
		delete[] segments;
		segments = new ObjSeg[segCount + 1];
		for (int i = 0; i < segCount; i++)
		{
			segments[i] = temp[i];
		}
		delete[] temp;
	}
	else
	{
		segments = new ObjSeg[1];
	}

	segments[segCount] = ObjSeg();
	segments[segCount].copy(o);
	segCount++;
}

void Object::add(Material mat)
{

	if (matCount > 0)
	{
		Material* temp = new Material[matCount];
		for (int i = 0; i < matCount; i++)
		{
			temp[i] = mats[i];
		}
		delete[] mats;
		mats = new Material[matCount + 1];
		for (int i = 0; i < matCount; i++)
		{
			mats[i] = temp[i];
		}
		delete[] temp;
	}
	else
	{
		mats = new Material[1];
	}
	mats[matCount] = mat;
	matCount++;
}

Material Object::findMat(std::string name)
{
	for (int i = 0; i < matCount; i++)
	{
		if (mats[i].name == name)
		{
			return mats[i];
		}
	}
	return Material();
}

void Object::updateSegments()
{
	for (int i = 0; i < segCount; i++)
	{
		segments[i].updateSegment();
		vec3 Vpos = segments[i].Maxs;
		if (Maxs.x < Vpos.x)
			Maxs.x = Vpos.x;

		if (Maxs.y < Vpos.y)
			Maxs.y = Vpos.y;

		if (Maxs.z < Vpos.z)
			Maxs.z = Vpos.z;

		Vpos = segments[i].Mins;

		if (Mins.x > Vpos.x)
			Mins.x = Vpos.x;

		if (Mins.y > Vpos.y)
			Mins.y = Vpos.y;

		if (Mins.z > Vpos.z)
			Mins.z = Vpos.z;
	}
}