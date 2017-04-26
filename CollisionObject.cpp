#include <iostream>
#include <fstream>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>

#include "Boid.h"
#include "CustomOperators.h"

void CollisionObject::getGeometry(vector<vec3> &vs, vector<vec3> &ns, vector<uint> &is)
{
  vs.clear();
  ns.clear();
  is.clear();

  for(vec3 v:this->vertices)
    vs.push_back(v);

  for(vec3 v:this->normals)
    ns.push_back(v);

  for(uint i:this->indices)
    is.push_back(i);
}

float sphereIntersection(vec3 ray, vec3 origin, vec3 center, float radius)
{

	float a = dot(ray,ray);
	float b = -2*dot(center, ray)+2*dot(ray,origin);
	float c = -2*dot(origin,center)+dot(center,center)
			  -radius*radius+dot(origin,origin);

	float discriminant =b*b - 4*a*c;

	float t1, t2;
	if(discriminant < 0)
	{
		return -1;
	}

	else
	{
		t1=(-b-sqrt(discriminant))/(2*a);
		t2=(-b+sqrt(discriminant))/(2*a);
	}

	if(t1<0 && t2>=0)
		t1=t2;
	else if (t1>=0 && t2<0)
		t2=t1;

	return glm::min(t1,t2);
}

#define EPSILON 0.000000000000000000000001f
vec3 Sphere::detectCollision(Boid *b)
{
  vec3 dir = normalize(b->velocity);
  vec3 offset = center - b->position;
  vec3 normal = normalize(offset);
  vec3 force = vec3(0);

  if(dot(b->velocity, b->velocity)>0 && (dot(normal, dir)>=0))
  {
    if(length(offset)<radius+10)
    {
      //b->position -= normal;
      return (-normal)*100.f;
    }

    offset -= normal*(radius+10);
    if(length(offset)<100 && length(offset)>EPSILON)
    {
      force = normal*(-1.f/(pow(length(offset),2.f)));
    }
  }

  return force;
}


vec3 CollisionObject::detectCollision(Boid *b){return vec3(0);}

#define U_DIVISIONS 50
#define V_DIVISIONS 50
Sphere::Sphere(vec3 c, float r)
{
  this->center = c;
  radius = r;

  float uStep = 1.f/((float)(U_DIVISIONS-1)*2);
	float vStep = 1.f/((float)(V_DIVISIONS-1));

	float u = 1/4.f;

	//Traversing u
	for(int i=0; i<U_DIVISIONS; i++)
	{

		float v = 0.f;
		//Traversing v
		for(int j=0; j<V_DIVISIONS; j++)
		{
			vec3 pos = vec3(r*sin(2.f*M_PI*v)*cos(2.f*M_PI*u),r*cos(2.f*M_PI*v)*cos(2.f*M_PI*u), r*sin(2.f*M_PI*u))+c;

			//cout<<v<<"\t"<<u<<endl;

			vec3 normal = normalize(pos);

			vertices.push_back(pos);
			normals.push_back(normal);
			//uvs.push_back(vec2(1-v,(u-1/4.f)*2));

			//cout<<v<<"\t"<<(u-1/4.f)*2<<endl;

			v += vStep;
		}

		u += uStep;
	}

	for(int i=0; i<U_DIVISIONS-1; i++)
	{
		for(int j=0; j<V_DIVISIONS -1; j++)
		{
			unsigned int p00 = i*V_DIVISIONS+j;
			unsigned int p01 = i*V_DIVISIONS+j+1;
			unsigned int p10 = (i+1)*V_DIVISIONS + j;
			unsigned int p11 = (i+1)*V_DIVISIONS + j + 1;

			indices.push_back(p00);
			indices.push_back(p01);
			indices.push_back(p10);
			indices.push_back(p11);
		}
	}
}
