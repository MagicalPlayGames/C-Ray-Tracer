#include <iomanip>
#include <fstream>
#include <sstream>

#include "glm-master/glm/gtx/transform.hpp"
#include "Ray.h"
#include "Light.h"
#include "Object.h"
#include "Texture.h"

using namespace std;
using namespace glm;

//Prototypes
int loadFile(string file);
void savePPM();
Texture loadPPM(string file);
void loadMat(string file, Object& obj);
Object loadObj(string file, mat4 O2W);
vec3 primaryRayColor(float, float);

Face findHitFace(Ray );
vec3 calcLighting(Ray, Face, int);
vec3 findTexture(Face, vec3);
vec3 findNormal(Face, vec3);

bool inShadow(vec3, vec3);
vec3 Reflection(Ray, int);

vec3 checkColor(vec3);

bool hitBox(Ray, Object);
bool hitSegBox(Ray, ObjSeg);

void Menu();

//Toggle variables
bool superSampling = false;
bool refracting = false;
bool reflecting = false;
bool fixCancer = false;
bool shadows = false;
float brightness = 0.8f;
int rayDepth = 3;



//Global Variables
glm::vec3* pixels;
int pSize = 0;
float d = 0.0f;

Light* lights;

Texture* textureArchive;
int archiveSize = 0;
Object* objs;

int oSize = 0;
int lSize = 0;


float Noffset = 0.00001f;


vec3 backgroundColor(0, 0, 0);
vec3 ambient(0, 0, 0);



int main(int argc, char* argv[])
{

	if (loadFile("1n.scn"))//loads the file name into the loadFile function
	{
		float yo = d - (d / float(pSize));
		float xo;
		for (int i = 0; i < pSize; i++)//For each row of pixels
		{
			xo = -d + (d / float(pSize));
			for (int j = 0; j < pSize; j++)//For each pixel in a row
			{
				vec3 color3 = primaryRayColor(xo, yo);//Shoot one primary Ray to find the pixel color
				if (superSampling)//Super Sampling approximates the pixel color by color3, and 4 corners (color1,2,4,5) weighted accordinally
				{
					float left = xo - (0.5f * d) / float(pSize);
					float right = xo + (0.5f * d) / float(pSize);
					float top = yo + (0.5f * d) / float(pSize);
					float bottom = yo - (0.5f * d) / float(pSize);

					vec3 color1 = primaryRayColor(left, top);
					vec3 color2 = primaryRayColor(right, top);
					vec3 color4 = primaryRayColor(left, bottom);
					vec3 color5 = primaryRayColor(right, bottom);

					pixels[pSize * i + j] = ((color1 * 0.25f) + (color2 * 0.25f) + color3 + (color4 * 0.25f) + (color5 * 0.25f)) / 2.0f;
				}
				else//If there is no super sampling, set the pixel color to color3
				{
					pixels[pSize * i + j] = color3;
				}
				xo += (2.0f * d) / float(pSize);
			}
			cout << float((1.0*i) / pSize) << endl;//This is a sort of progress output. 0 = 0%, 1 = 100% of the render is loaded
			yo -= (2.0f * d) / float(pSize);
		}
		savePPM();//Save picture as a ppm
	}
	return 0;
}

//Shoots a primary ray from (xOff,yOff) location of the screen
vec3 primaryRayColor(float xOff, float yOff)
{
	vec3 camera(0, 0, 1);
	vec3 direction(xOff, yOff, -1);

	//Default color is the color of the background
	vec3 color = backgroundColor;

	//Creates a ray from the camera towards direction
	Ray primaryRay(camera, normalize(direction));
	//Finds the first face that the ray hits
	Face hit = findHitFace(primaryRay);
	//fNormal is sest to vec3(-1) by default, so fNormal is vec3(-1) if the ray does not hit a face
	if (hit.fNormal != vec3(-1))
	{
		//Sets distance t and hitpoint of the primary ray
		primaryRay.tWorld = hit.curT;
		primaryRay.calcHitPoint();
		
		//Calculates the lighting and color at the point the ray hits
		color = calcLighting(primaryRay, hit, 0);
	}
	//Changes color fro (0.0-1.0) to (0-255)
	color *= 255.0f;

	return color;
}

//Shoots a ray off a surface to create a relfected image and increaments the number of rays casted
vec3 Reflection(Ray startingRay,int secondaryRayDepth)
{
	vec3 reflectedColor = backgroundColor;

	Face reflectFace = findHitFace(startingRay);
	if (reflectFace.fNormal != vec3(-1))
	{
		Ray reflectionRay(startingRay.uWorld, startingRay.vWorld);
		reflectionRay.tWorld = reflectFace.curT;
		reflectionRay.calcHitPoint();

		if (reflectionRay.getT() > 0)
			reflectedColor = calcLighting(reflectionRay, reflectFace,secondaryRayDepth + 1);
	}
	return reflectedColor;
}

//Calculates the color of the ray curRay, the face hit focus for each curRay passed through
vec3 calcLighting(Ray curRay, Face focus, int secondaryRayDepth)
{

	vec3 clr(0, 0, 0);
	vec3 colorBuffer(0, 0, 0);
	//Variables for ambient, diffuse, and specular light
	vec3 Ka(1);
	vec3 Kd(1);
	vec3 Ks(1);

	vec3 N;
	Material m;
	//If the face has a material attached, it works off of those variables
	if (focus.matName != "")
	{
		int i = focus.objIndex;
			m = objs[i].findMat(focus.matName);
			if (m.name != "")
			{
				Ka = m.Ka;
				Kd = m.Kd;
				Ks = m.Ks;
			}
	}
	//If Material m has a normal map, it uses that normal map for normals
	if (m.bumped)
	{
			//texNormal is the normal accordding to the normal map
			vec3 texNormal = findNormal(focus, curRay.hitpointWorld);
			// angle is the angle between the normal of the face and the texNormal
			float angle = dot(vec3(0, 0, 1), focus.fNormal);
			//changes the angle of texNormal to have the appropriate texture
			texNormal = texNormal * angle;
			N = normalize(texNormal);

	}
	else
	{
		N = normalize(vec4(focus.fNormal, 0));
	}

	//For each light
	for (int j = 0; j < lSize; j++)
	{
		vec3 lightPosition = lights[j].position;
		vec3 lightColor = lights[j].color;

		vec3 diffuse(0, 0, 0);
		vec3 specular(0, 0, 0);
		vec3 reflectedColor(0, 0, 0);
		vec3 refractedColor(0, 0, 0);

		//Pulls the point off the surface to prevent errors in the next rays
		vec3 elevatedHitPoint = vec4(curRay.hitpointWorld, 1) + vec4((Noffset * N), 1);

		//If the new point is not in a shadow, calculate lighting
		if (!inShadow(elevatedHitPoint, lightPosition - elevatedHitPoint))
		{
		//Ray from light to the hit point
		vec3 L = normalize(vec4(lightPosition,1) - vec4(curRay.hitpointWorld,1));

		//If there is reflection, set the default as the reflectiveness times the background 
		if(reflecting && m.reflective)
			reflectedColor = Ks*backgroundColor;
		//If there is reflection, and there are not too many previous rays
		if (secondaryRayDepth < rayDepth && reflecting && m.reflective)
		{
			//Calculate reflection
			vec3 ReflectV = reflect(curRay.vWorld, N);
			Ray reflectRay(elevatedHitPoint, ReflectV);
			reflectedColor = Ks * Reflection(reflectRay, secondaryRayDepth);
		}

			//If there is no diffuse, there is no specular
			float diffuseCheck = dot(L, N);
			if (diffuseCheck > 0.0f)
			{
				//If there is a texture, use that as the default color
				if (m.textured)
				{
					diffuse = (findTexture(focus, curRay.hitpointWorld) * Kd * diffuseCheck);
				}
				//else use the diffuse K as the default color
				else
					diffuse = Kd * diffuseCheck;

				vec3 R = reflect(-L, N);

				float specularCheck = dot(-curRay.vWorld, R);
				if(specularCheck<0.0f)
					specularCheck = dot(curRay.vWorld, R);

				//There are no specular maps in this program
				if (specularCheck > 0.0f)
				{
					specular = Ks * pow(specularCheck, m.Ns);
				}

				diffuse = checkColor(diffuse);
				specular = checkColor(specular);
				//adds the color, because there are multiple possible lights
				colorBuffer += (diffuse+specular) * lightColor;

			}
		}
		reflectedColor = checkColor(reflectedColor);

		colorBuffer += (refractedColor + reflectedColor) * lightColor;
	}
	clr = checkColor((ambient*Ka) + colorBuffer);
	//Always check to make sure the color variables are between 0.0 and 1.0

	return clr;
}

//Shoots a ray towards the light to see if there is an object inbetween the starting point and the light
bool inShadow(vec3 u, vec3 v)
{
	if (!shadows)
		return false;
	Ray shadowRay(u, v);
		Face f = findHitFace(shadowRay);
		if (f.fNormal!=vec3(-1))
		{
				return true;
		}
		return false;

}

//These next two functions deal with bounding boxes. This optimizes the code by skipping a lot of processing
//if it does not hit a bounding box

//A check to make sure the hitpoint of the ray is in the hitbox of the object
bool hitBox(Ray r, Object obj)
{

	float hitz = obj.Maxs.z;
	float t = (obj.Maxs.z - r.uWorld.z) / r.vWorld.z;

	if (r.uWorld.x <= obj.Maxs.x && r.uWorld.x >= obj.Mins.x && r.uWorld.y <= obj.Maxs.y && r.uWorld.y >= obj.Mins.y && r.uWorld.z <= obj.Maxs.z && r.uWorld.z >= obj.Mins.z)
	{
		return true;
	}

	if (t > 0.0f)
	{
		vec3 hitpoint = r.uWorld + (r.vWorld * t);
		if (hitpoint.x <= obj.Maxs.x && hitpoint.x >= obj.Mins.x && hitpoint.y <= obj.Maxs.y && hitpoint.y >= obj.Mins.y && hitpoint.z <= obj.Maxs.z && hitpoint.z >= obj.Mins.z)
		{
			return true;
		}
	}
	hitz = obj.Mins.z;
	t = (obj.Mins.z - r.uWorld.z) / r.vWorld.z;
	if (t > 0.0f)
	{
		vec3 hitpoint = r.uWorld + (r.vWorld * t);
		if (hitpoint.x <= obj.Maxs.x && hitpoint.x >= obj.Mins.x && hitpoint.y <= obj.Maxs.y && hitpoint.y >= obj.Mins.y && hitpoint.z <= obj.Maxs.z && hitpoint.z >= obj.Mins.z)
		{
			return true;
		}
	}
	hitz = (obj.Mins.z+obj.Maxs.z)/2.0f;
	t = ((obj.Mins.z + obj.Maxs.z) / 2.0f) / r.vWorld.z;
	if (t > 0.0f)
	{
		vec3 hitpoint = r.uWorld + (r.vWorld * t);
		if (hitpoint.x <= obj.Maxs.x && hitpoint.x >= obj.Mins.x && hitpoint.y <= obj.Maxs.y && hitpoint.y >= obj.Mins.y && hitpoint.z <= obj.Maxs.z && hitpoint.z >= obj.Mins.z)
		{
			return true;
		}
	}
	return false;
}

//A check to make sure the hitpoint of the ray is in the hitbox of the segment
bool hitSegBox(Ray r, ObjSeg obj)
{

	float hitz = obj.Maxs.z;
	float t = (obj.Maxs.z - r.uWorld.z) / r.vWorld.z;

	if (r.uWorld.x <= obj.Maxs.x && r.uWorld.x >= obj.Mins.x && r.uWorld.y <= obj.Maxs.y && r.uWorld.y >= obj.Mins.y && r.uWorld.z <= obj.Maxs.z && r.uWorld.z >= obj.Mins.z)
	{
		return true;
	}

	if (t > 0.0f)
	{
		vec3 hitpoint = r.uWorld + (r.vWorld * t);
		if (hitpoint.x <= obj.Maxs.x && hitpoint.x >= obj.Mins.x && hitpoint.y <= obj.Maxs.y && hitpoint.y >= obj.Mins.y && hitpoint.z <= obj.Maxs.z && hitpoint.z >= obj.Mins.z)
		{
			return true;
		}
	}
	hitz = obj.Mins.z;
	t = (obj.Mins.z - r.uWorld.z) / r.vWorld.z;
	if (t > 0.0f)
	{
		vec3 hitpoint = r.uWorld + (r.vWorld * t);
		if (hitpoint.x <= obj.Maxs.x && hitpoint.x >= obj.Mins.x && hitpoint.y <= obj.Maxs.y && hitpoint.y >= obj.Mins.y && hitpoint.z <= obj.Maxs.z && hitpoint.z >= obj.Mins.z)
		{
			return true;
		}
	}
	hitz = (obj.Mins.z + obj.Maxs.z) / 2.0f;
	t = ((obj.Mins.z + obj.Maxs.z) / 2.0f) / r.vWorld.z;
	if (t > 0.0f)
	{
		vec3 hitpoint = r.uWorld + (r.vWorld * t);
		if (hitpoint.x <= obj.Maxs.x && hitpoint.x >= obj.Mins.x && hitpoint.y <= obj.Maxs.y && hitpoint.y >= obj.Mins.y && hitpoint.z <= obj.Maxs.z && hitpoint.z >= obj.Mins.z)
		{
			return true;
		}
	}
	return false;
}


//Finds the next face Ray r hits
Face findHitFace(Ray r)
{
	float smallestT = FLT_MAX;
	int index = -1;
	int selectedDepth = -1;
	int selectedobj = -1;
	int selectedSeg = -1;
	for (int w = 0; w < oSize;w++)//for each object, make sure it hits its bounding box
	{
		if (hitBox(r, objs[w]))
		{
			for (int j = 0; j < objs[w].segCount; j++)//for each object hit, check to see if it hits the bounding box of an object segment
			{
				if (hitSegBox(r, objs[w].segments[j]))
				{
					int depth = 0;
					//ObjSegments contain lists of object segments and bounding boxes inside of the current object segment
					ObjSeg objSeg = objs[w].segments[j];
					ObjSeg* nextSeg = &objSeg;
					while(nextSeg!=nullptr)
					{
						ObjSeg c;
						c.copy(nextSeg);
						if (!hitSegBox(r, c))
						{
							break;
						}
						for (int i = 0; i < nextSeg->numOfFs; i++)//Calculate the distance t of the ray starting point to hitpoint for each face
						{
							Face temp = nextSeg->faces[i];
							vec3 e1 = temp.verticies[1].position - temp.verticies[0].position;
							vec3 e2 = temp.verticies[2].position - temp.verticies[0].position;
							vec3 pvec = cross(r.vWorld, e2);
							float a = dot(e1, pvec);

							if (a > 0.0000001f)
							{

								float f = 1.0f / a;//inverse of proposed t

								vec3 vt = r.uWorld - temp.verticies[0].position;
								float u = f * dot(vt, pvec);
								if (u >= 0.0f && u <= 1.0f)
								{

									vec3 qvec = cross(vt, e1);
									float v = dot(r.vWorld, qvec) * f;

									if (v >= 0.0f && u + v <= 1.0f)
									{
										float t = dot(e2, qvec) * f;

										if (t > 0 && t < smallestT)//finds the smallest t for the ray of each face of each segment of each object hit
										{
											smallestT = t;
											index = i;
											selectedDepth = depth;
											selectedobj = w;
											selectedSeg = j;
										}
									}
								}
							}
						}
						depth++;
						nextSeg = nextSeg->nextSeg;
					}
				}
			}
		}
	}
	if (index == -1)
	{
		Face tr;
		return  tr;
	}
	else
	{
		ObjSeg* selected = &objs[selectedobj].segments[selectedSeg];
		for (int i = 0; i < selectedDepth; i++)
		{
			selected = selected->nextSeg;
		}
		selected->faces[index].curT = smallestT;
		return selected->faces[index];
	}
}

//Returns the valule of the color where r,g, and b are less than 1.0
vec3 checkColor(vec3 clr)
{
	if (clr.r > 1.0f)
	{
		clr.r = 1.0f;
	}
	else if (clr.r < 0.0f)
	{
		clr.r = 0.0f;
	}

	if (clr.g > 1.0f)
	{
		clr.g = 1.0f;
	}
	else if (clr.g < 0.0f)
	{
		clr.g = 0.0f;
	}

	if (clr.b > 1.0f)
	{
		clr.b = 1.0f;
	}
	else if (clr.b < 0.0f)
	{
		clr.b = 0.0f;
	}

	return clr;
}

//loads all the aspects of file
//Use readme for more information
int loadFile(string file)
{
	fstream input;
	input.open(file);

	cout << setprecision(4);

	vec3 diff(0.0f);
	vec3 spec(0.0f);
	mat4 O2W = mat4(1.0f);
	mat4 W2O = mat4(1.0f);
	mat4 ungroupedO2W = mat4(1.0f);
	mat4 ungroupedW2O = mat4(1.0f);
	vec3 refract = vec3(0.0f);
	float shininess = 0.0f;
	float curMaxDistance = 0.0f;
	float tempMaxDistance = 0.0f;
	float beforeMaxDistance = 0.0f;

	if (input.is_open() == true)
	{
		cout << "File loaded, please wait\n";
		string nextCom;

		while (input >> nextCom)
		{
			if (nextCom == "view")
			{
				input >> pSize >> d;
			}
			else if (nextCom[0] == '#')
			{
				//skip input
			}
			else if (nextCom == "Object")
			{
				if (oSize > 0)
				{
					Object* temp = new Object[oSize];
					for (int i = 0; i < oSize; i++)
					{
						temp[i] = objs[i];
					}
					delete[] objs;
					objs = new Object[oSize + 1];
					for (int i = 0; i < oSize; i++)
					{
						objs[i] = temp[i];
					}
					delete[] temp;
				}
				else
				{
					objs = new Object[1];
				}
				string file;
				input >> file;
				objs[oSize] = loadObj(file,O2W);
				oSize++;
			}
			else if (nextCom == "scale")
			{
				vec3 temp;
				input >> temp.x >> temp.y >> temp.z;

				O2W = O2W * scale(mat4(1.0f), temp);
				W2O = inverse(scale(mat4(1.0f), temp)) * W2O;
				tempMaxDistance += temp.z / 2.0f;
			}
			else if (nextCom == "move")
			{
				vec3 temp;
				input >> temp.x >> temp.y >> temp.z;
				O2W = O2W * translate(mat4(1.0f), temp);
				W2O = translate(mat4(1.0f), -temp) * W2O;
				tempMaxDistance -= temp.z;
			}
			else if (nextCom == "rotate")
			{
				vec3 temp;
				float angle;
				input >> angle >> temp.x >> temp.y >> temp.z;
				O2W = O2W * rotate(mat4(1.0), angle * 3.141f / 180.0f, temp);
				W2O = inverse(rotate(mat4(1.0), angle * 3.141f / 180.0f, temp)) * W2O;
			}
			else if (nextCom == "light")
			{
				float r, g, b, x, y, z;
				input >> r >> g >> b >> x >> y >> z;
				if (lSize > 0)
				{
					Light* temp = new Light[lSize];
					for (int i = 0; i < lSize; i++)
					{
						temp[i] = lights[i];
					}
					delete[] lights;
					lights = new Light[lSize + 1];
					for (int i = 0; i < lSize; i++)
					{
						lights[i] = temp[i];
					}
					delete[] temp;
				}
				else
				{
					lights = new Light[1];
				}
				lights[lSize].setup(r * brightness, g * brightness, b * brightness, x, y, z);
				lSize++;

			}
			else if (nextCom == "background")
			{
				input >> backgroundColor.r >> backgroundColor.g >> backgroundColor.b;
				backgroundColor *= brightness;
			}
			else if (nextCom == "ambient")
			{
				input >> ambient.r >> ambient.g >> ambient.b;
				ambient *= brightness;
			}
			else if (nextCom == "group")
			{
				ungroupedO2W = O2W;
				ungroupedW2O = W2O;
				beforeMaxDistance = tempMaxDistance;
			}
			else if (nextCom == "groupend")
			{
				O2W = ungroupedO2W;
				W2O = ungroupedW2O;
				if (curMaxDistance < tempMaxDistance && tempMaxDistance>beforeMaxDistance)
				{
					curMaxDistance = tempMaxDistance;
				}
				else if(curMaxDistance<beforeMaxDistance)
				{
					curMaxDistance = beforeMaxDistance;
				}
				tempMaxDistance = beforeMaxDistance;
			}
			else if (nextCom == "material")
			{
				input >> diff.r >> diff.g >> diff.b;
				input >> spec.r >> spec.g >> spec.b >> shininess;
			}
		}



		pixels = new vec3[pSize * pSize];
		for (int i = 0; i < pSize * pSize; i++)
		{
			pixels[i] = vec3(0, 0, 0);
		}

		input.close();

		cout << "Visit Edit Menu? Y/N" << endl;

		string answer;
		cin >> answer;

		cout << endl;

		if (answer == "Y" || answer == "y")
		{
			Menu();
			cout << endl;
		}

		cout << "Rendering" << endl;
		delete[] textureArchive;
		archiveSize = 0;
		return 1;
	}
	else
	{
		cout << "File failed to open\n";
		input.close();
		return -1;
	}
}

//Saves render as a ppm P3 format
void savePPM()
{
	fstream output;
	output.open("out.ppm", fstream::out);
	output << "P3\n" << "# Raytracer out.ppm\n" << pSize << " " << pSize << " 255\n";
	for (int i = 0; i < pSize; i++)
	{
		for (int j = 0; j < pSize; j++)
		{
			output << pixels[pSize * i + j].r << " " << pixels[pSize * i + j].g << " " << pixels[pSize * i + j].b << " ";
			output << "\t";
		}
		output << endl;
	}
	output.close();
	cout << "File Done.";
}

//Menu option at the beginning to toggle extra variables
void Menu()
{
	int choice = -1;
	while (choice != 8)
	{
		cout << "Toggle one of the following options" << endl;

		cout << "1. Super Sampling: ";
		if (superSampling)
			cout << "Enabled" << endl;
		else
			cout << "Disabled" << endl;

		cout << "2. Refracting: ";
		if (refracting)
			cout << "Enabled" << endl;
		else
			cout << "Disabled" << endl;

		cout << "3. Reflecting: ";
		if (reflecting)
			cout << "Enabled" << endl;
		else
			cout << "Disabled" << endl;

		cout << "4. Calculate out Cancer: ";
		if (fixCancer)
			cout << "Enabled" << endl;
		else
			cout << "Disabled" << endl;

		cout << "5. Shadows: ";
		if (shadows)
			cout << "Enabled" << endl;
		else
			cout << "Disabled" << endl;

		cout << "6. Brightness: " << brightness << endl;
		cout << "7. Secondary Ray Depth: " << rayDepth << endl;
		cout << "8. Start Render " << endl;

		cout << endl;



		cin >> choice;
		switch (choice)
		{
		case 0:
		case 1:
			superSampling = !superSampling;
			cout << "Super Sampling ";
			if (superSampling)
				cout << "Enabled" << endl;
			else
				cout << "Disabled" << endl;
			break;
		case 2:
			refracting = !refracting;
			cout << "Refractions ";
			if (refracting)
				cout << "Enabled" << endl;
			else
				cout << "Disabled" << endl;
			break;
		case 3:
			reflecting = !reflecting;
			cout << "Reflections ";
			if (reflecting)
				cout << "Enabled" << endl;
			else
				cout << "Disabled" << endl;
			break;
		case 4:
			fixCancer = !fixCancer;
			cout << "Cancer Calculations ";
			if (fixCancer)
				cout << "Enabled" << endl;
			else
				cout << "Disabled" << endl;
			break;
		case 5:
			shadows = !shadows;
			cout << "Shadows ";
			if (shadows)
				cout << "Enabled" << endl;
			else
				cout << "Disabled" << endl;
			break;
		case 6:
			cout << "Define brightness as a float from 0 to 1" << endl;
			cin >> brightness;
			break;
		case 7:
			cout << "Define secondary ray depth as a positive int" << endl;
			cin >> rayDepth;
			break;
		}
	}
}

//loads file ppm to use as a normal map or texture
Texture loadPPM(string file)
{
	Texture tex;
	fstream input;
	input.open(file);
	if (input.is_open())
	{
		std::cout << "Loading " << file << " Texture" << std::endl;
		string nextCom;
		int width;
		int height;

		input >> width;
		input >> height;
		vec3* texturePixels = new vec3[height * width];
		int w = 0;
		int h = 0;
		while (input >> nextCom)
		{
			input >> nextCom;
			int r;
			int g;
			int b;
			char c;
			istringstream comc(nextCom);
			comc >> c >> r >> c >> g >> c >> b >> c;

			vec3 clr(r, g, b);
			w++;
			if (w >= width)
			{
				w = 0;
				h+=width;
			}
			texturePixels[h + w] = clr;
			input >> nextCom >> nextCom;
		}
		tex.setup(texturePixels, width, height,file);
	}
	else
	{
		cout << "File failed to load\n";
		input.close();
	}
	return tex;
}

//Finds the normal of focus according to the normal map
vec3 findNormal(Face focus, vec3 hitpoint)
{

	float xRatio = (hitpoint.x - focus.Pmins.x) / (focus.Pmaxs.x - focus.Pmins.x);
	float yRatio = (hitpoint.y - focus.Pmins.y) / (focus.Pmaxs.y - focus.Pmins.y);
	vec2 uv = vec2((xRatio * (focus.Tmaxs.x - focus.Tmins.x)) + focus.Tmins.x, (yRatio * (focus.Tmaxs.y - focus.Tmins.y)) + focus.Tmins.y);
	Material m = objs[focus.objIndex].findMat(focus.matName);
	int width = m.bW - (uv.y * m.bW);
	int height = uv.x * m.bH;
	int place = (width * m.bH) + (height);
	vec3 color0 = m.bump[place] / 255.0f;
	return color0;
}

//Finds the texture of focus according to the texture map
vec3 findTexture(Face focus, vec3 hitpoint)
{
	Material m = objs[focus.objIndex].findMat(focus.matName);
	float xRatio = (hitpoint.x - focus.Pmins.x) / (focus.Pmaxs.x - focus.Pmins.x);
	float yRatio = (hitpoint.y - focus.Pmins.y) / (focus.Pmaxs.y - focus.Pmins.y);
	vec2 uv = vec2((xRatio * (focus.Tmaxs.x - focus.Tmins.x)) + focus.Tmins.x, (yRatio * (focus.Tmaxs.y - focus.Tmins.y)) + focus.Tmins.y);
	int width = m.textureW-(uv.y * m.textureW);
	int height = uv.x *m.textureH;
	int place = (width* m.textureH) + (height);
	vec3 color0 = m.texture[place]/255.0f;
	return color0;
}

//loads .obj file
//It finds all the verticies, vertex texture cordinates, and normal coordinates
//It uses this information to create a face with attributes.
//Each face is put into object segments for the object
Object loadObj(string file, mat4 O2W)
{
	fstream input;
	input.open(file);
	string matFile;
	string objName = "";
	string matName = "";
	Vertex temp;
	Vertex* faceVs;
	Object final;
	ObjSeg* obj = new ObjSeg();
	ObjSeg* first = nullptr;
	float vStart = 0;
	float vTStart = 0;
	float vNStart = 0;
	if (input.is_open())
	{
		std::cout << "Loading " << file << " File" << std::endl;
		string nextCom;
		while (input >> nextCom)
		{
			if (nextCom == "mtllib")
			{
				input >> matFile;
				loadMat(matFile, final);

			}
			else if (nextCom == "usemtl")
			{
				input >> matName;
			}
			else if (nextCom == "o")
			{
				if (objName != "")
				{
					obj->splitFaces(-1, obj);
					obj->numOfFs = 0;
					final.add(obj);
					vStart += first->numOfVs;
					vTStart += first->numOfTPs;
					vNStart += first->numOfNs;

					delete[] obj->verticies;
					delete[] obj->normals;
					delete[] obj->texPos;
					first = nullptr;
					obj = new ObjSeg();
					matName = "";
				}
				input >> objName;
				std::cout << objName << std::endl;
			}
			else if (nextCom == "v")
			{
				float x, y, z;
				input >> x >> y >> z;
				temp.addPosition(vec3(x, y, z));
				if (O2W != mat4(1.0f))
				{
					temp.changePosition(O2W);
				}

				obj->addVertex(temp);
			}
			else if (nextCom == "vt")
			{
				float x, y;
				input >> x >> y;
				obj->addtexPos(vec2(x, y));
			}
			else if (nextCom == "vn")
			{
				float x, y, z;
				input >> x >> y >> z;
				obj->addNormal(vec3(x, y, z));
			}
			else if (nextCom == "f")
			{
				faceVs = new Vertex[3];
				for (int i = 0; i < 3; i++)
				{
					input >> nextCom;
					float vertexI = 0;
					float texPosI = -1;
					float normalI = 0;

					int start = 0;
					int end = nextCom.find('/', start);
					string v = nextCom.substr(start, end);
					stringstream comV(v);
					comV >> vertexI;

					start = end + 1;
					end = nextCom.find('/', start);
					if (end != start)
					{
						v = nextCom.substr(start, end - start);
						stringstream comT(v);
						comT >> texPosI;
					}
					start = end;
					v = nextCom.substr(start + 1);
					stringstream comN(v);
					comN >> normalI;

					Vertex vert;
					if (first == nullptr)
						first = obj;
					vec3 pos = first->verticies[long(vertexI - vStart - 1)].position;

					vec3 normal = first->normals[long(normalI - vNStart - 1)];
					vec2 texPos(-1);
					if (texPosI != -1)
						texPos = first->texPos[long(texPosI - vTStart - 1)];

					vert.setup(pos, normal, texPos);
					faceVs[i] = vert;
				}
				if (obj->numOfFs > 100)
				{
					obj->splitFaces(-1, obj);
					final.add(obj);
					obj = new ObjSeg();
					if (first != obj)
					{
						delete[] obj->verticies;
						delete[] obj->normals;
						delete[] obj->texPos;
					}
				}
				obj->addFace(faceVs);
				obj->faces[obj->numOfFs - 1].setTree(oSize, final.segCount - 1);
				if (matName != "")
				{
					obj->faces[obj->numOfFs - 1].setMat(matName);
				}
			}
		}
		obj->splitFaces(-1, obj);
		obj->numOfFs = 0;
		final.add(obj);
		final.updateSegments();


	}
	else
	{
		cout << "File failed to load\n";
		input.close();
	}
	return final;
}

//loads the material from the .obj file, and stores it
void loadMat(string file, Object& obj)
{

	string name = "";
	float Ns = -1;
	vec3 Ka = vec3(-1);
	vec3 Kd = vec3(-1);
	vec3 Ks = vec3(-1);
	vec3 Ke = vec3(-1);
	float Ni = -1;
	float d = -1;

	fstream input;
	input.open(file);
	Material mat;
	if (input.is_open())
	{
		std::cout << "Loading " << file << " File" << std::endl;
		string nextCom;
		while (input >> nextCom)
		{
			if (nextCom == "newmtl")
			{
				if (name != "")
				{
					mat.setup(name, Ns, Ka, Kd, Ks, Ke, Ni, d);
					obj.add(mat);
					mat = Material();
					mat.reflective = false;
				}
				input >> name;
			}
			else if (nextCom == "reflective")
			{
				mat.reflective = true;
			}
			else if (nextCom == "map_Kd" || nextCom == "map_Bump")
			{
				string map;
				input >> map;
				map = map.substr(0, map.length() - 3);
				map += "ppm";
				Texture tex;
				bool found = false;
				for (int i = 0; i < archiveSize; i++)
				{
					if (map == textureArchive[i].name)
					{
						tex = textureArchive[i];
						found = true;
						break;
					}
				}
				if (!found)
				{
					tex = loadPPM(map);
					if (archiveSize > 0)
					{
						Texture* temp = new Texture[archiveSize];
						for (int i = 0; i < archiveSize; i++)
						{
							temp[i] = textureArchive[i];
						}
						delete[] textureArchive;
						textureArchive = new Texture[archiveSize + 1];
						for (int i = 0; i < archiveSize; i++)
						{
							textureArchive[i] = temp[i];
						}
						delete[] temp;
					}
					else
					{
						textureArchive = new Texture[1];
					}
					textureArchive[archiveSize] = tex;
					archiveSize++;
				}
				if(nextCom == "map_Kd")
					mat.setTexture(tex.texture,tex.textureH,tex.textureW);
				else if (nextCom == "map_Bump")
					mat.setBump(tex.texture, tex.textureH, tex.textureW);
			}
			else if (nextCom == "Ns")
			{
				input >> Ns;
			}
			else if (nextCom == "Ka")
			{
				float x, y, z;
				input >> x >> y >> z;
				Ka = vec3(x, y, z);
			}
			else if (nextCom == "Kd")
			{
				float x, y, z;
				input >> x >> y >> z;
				Kd = vec3(x, y, z);
			}
			else if (nextCom == "Ks")
			{
				float x, y, z;
				input >> x >> y >> z;
				Ks = vec3(x, y, z);
			}
			else if (nextCom == "Ke")
			{
				float x, y, z;
				input >> x >> y >> z;
				Ke = vec3(x, y, z);
			}
			else if (nextCom == "Ni")
			{
				input >> Ni;
			}
			else if (nextCom == "d")
			{
				input >> d;
			}
		}
		mat.setup(name, Ns, Ka, Kd, Ks, Ke, Ni, d);
		obj.add(mat);
	}
	else
	{
	cout << "File failed to load\n";
	input.close();
	}
}