#include "Face.h"

using namespace glm;

void Face::setup(Vertex* v)
{
	verticies = v;
	for (int i = 0; i < 3; i++)//transliation hack
	{
		vec2 VposT = verticies[i].texturePosition;
		if (Tmins.x > VposT.x)
			Tmins.x = VposT.x;
		if (Tmins.y > VposT.y)
			Tmins.y = VposT.y;


		if (Tmaxs.x < VposT.x)
			Tmaxs.x = VposT.x;
		if (Tmaxs.y < VposT.y)
			Tmaxs.y = VposT.y;

		vec3 Vpos = verticies[i].position;//points not relative to the z axis, since uv doesn't have a z

		if (Pmins.x > Vpos.x)
			Pmins.x = Vpos.x;
		if (Pmins.y > Vpos.y)
			Pmins.y = Vpos.y;

		if (Pmaxs.x < Vpos.x)
			Pmaxs.x = Vpos.x;
		if (Pmaxs.y < Vpos.y)
			Pmaxs.y = Vpos.y;
	}
	//vec3 e1 = verticies[1].position - verticies[0].position;
	//vec3 e2 = verticies[2].position - verticies[0].position;
	//if(smooth, else use cross(e1,e2)
	vec3 normal = (verticies[0].normal+verticies[1].normal+verticies[2].normal)/3.0f;
	fNormal = normal;
}

void Face::setTree(int obj, int seg)
{
	objIndex = obj;
	objSegIndex = seg;
}

void Face::setMat(std::string f)
{
	matName = f;
}