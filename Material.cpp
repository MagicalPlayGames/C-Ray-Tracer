#include "Material.h"
using namespace glm;

void Material::setup(std::string n, float ns, vec3 ka, vec3 kd, vec3 ks, vec3 ke, float ni, float D)
{
	name = n;
	Ns = ns;
	Ka = ka;
	Kd = kd;
	Ks = ks;
	Ke = ke;
	Ni = ni;
	d = D;
}

void Material::setTexture(vec3* tex, int tH, int tW)
{
	texture = new vec3[tH * tW];
	for (int i = 0; i < tH * tW; i++)
	{

		texture[i] = tex[i];
	}
	textured = true;
	textureH = tH;
	textureW = tW;
}

void Material::setBump(vec3* tex, int tH, int tW)
{
	bump = new vec3[tH * tW];
	for (int i = 0; i < tH * tW; i++)
	{

		bump[i] = tex[i];
	}
	bumped = true;
	bH = tH;
	bW = tW;
}