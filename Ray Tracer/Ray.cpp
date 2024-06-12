#include "Ray.h"


	Ray::Ray(vec3 startPoint, vec3 direction)
	{
		uWorld = vec4(startPoint,1);
		vWorld = vec4(direction,0);
		hitpointWorld = vec3(0,0,0);


		tWorld = calcT(uWorld,vWorld);
	}


	void Ray::calcHitPoint()
	{
		hitpointWorld = uWorld + (vWorld * tWorld);
	}

	float Ray::getT()
	{

		return tWorld;
	}


	float Ray::calcT(vec3 u, vec3 v)
	{
		double a = dot(v, v);
		double b = 2.0 * dot(u, v);
		double c = dot(u, u) - 1.0;
		double d = b * b - 4 * a * c;

		float tFinal = FLT_MAX;
		if (d > 0)
		{
			double t1 = 0.5 * (-b - sqrt(d)) / a;
			double t2 = 0.5 * (-b + sqrt(d)) / a;
			if (t1 > 0 && t2 > 0)
			{
				tFinal = std::min(t1, t2);
			}
			else if (t1 > 0)
			{
				tFinal = t1;
			}
			else if (t2 > 0)
			{
				tFinal = t2;
			}
		}
		return tFinal;
	}


