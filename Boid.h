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

using namespace std;
using namespace glm;

#define EPSILON 0.000000000000000000000001f

class Flock;
class Boid;
class CollisionObject;
class Sphere;

class CollisionObject
{
public:
  vector<vec3> vertices;
  vector<vec3> normals;
  vector<uint> indices;

  void getGeometry(vector<vec3> &vs, vector<vec3> &nsls, vector<uint> &is);
  virtual vec3 detectCollision(Boid* b);
};

class Sphere:public CollisionObject
{
public:
  vec3 center;
  float radius;

  Sphere(vec3 center, float r);
  vec3 detectCollision(Boid* b);
};

class Boid
{
public:
  Flock *myFlock = NULL;

  float prevT=0;

  vec3 forward = vec3(0,1,0);
  vec3 right = vec3(1,0,0);
  vec3 up = vec3(0,0,1);

  vec3 netAcceleration;
  vec3 prevAcceleration = vec3(0);

  vec3 position;
  vec3 nextPos = vec3(0);
  vec3 velocity = vec3(0);
  vec3 nextVel;

  int x=0, y=0, z=0;

  Boid(vec3 p, vec3 v, vec3 f);
  Boid(vec3 p, vec3 v);
  Boid(vec3 p);
  Boid();

  void calculateMovement(float t);
  void behave(vector<Boid*> &boids);
  void hashBehave();
  void update();

  bool detect(Boid* b);

  mat4 getModelMatrix();

private:
  vec3 ruleSep(vector<Boid*> &boids);
  vec3 ruleAl(vector<Boid*> &boids);
  vec3 ruleCohesion(vector<Boid*> &boids);
  vec3 ruleHerd();
  vec3 ruleCounterHerd();
  vec3 ruleCollisons();
};


//Hey, this isn't a boid... it should be in a flock.h or something..
class Flock
{
public:
  ///vector<vector<vector<vector<Boid*>>>>  grid;
  static vector<CollisionObject*> objects;

  vector<Boid*> boids;
  vector<vector<Boid*>> hashTable;

  float maxAcceleration = 10;
  float maxVelocity = 20;
  float fov = 320;

  float sepRadius = 10;
  float alignmentRadius = 15;
  float cohesionRadius = 25;

  float sepCoeff = 1;
  float alignmentCoeff = 0.001;
  float cohesionCoeff = 0.5;

  bool herding = false;
  bool cHerding = false;

  vec3 center;
  vec3 herdPoint;
  vec3 color = vec3(1,0.3,0);
  float radius = 500;

  Flock(int boidNum);
  Flock(string fileName);

  void update(float t);
  //void subdivide(vector<Boid*> &boids);
  void allocate();
  void hash(Boid *b);
  vector<Boid*> getBoids(int i, int j, int k);
};
