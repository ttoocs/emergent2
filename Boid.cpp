#include "Boid.h"

bool Boid::detect(Boid *b)
{
  float view = (2.f*M_PI/360.f)*(myFlock->fov/2.f);

  vec3 v = vec3(0), dir = vec3(0);
  if(length(velocity)>EPSILON)
    v=normalize(velocity);
  dir = b->position-position;
  if(length(dir)>EPSILON)
    dir = normalize(dir);
  float angle = acos(dot(v,dir));
  if(angle < view)
    return true;
  return false;
}

void Boid::hashBehave()
{
  for(int i=-1; i<=1; i++)
  {
    if(x+i>=0)
    {
      for(int j=-1; j<=1; j++)
      {
        if(y+j>=0)
        {
          for(int k=-1; k<=1; k++)
          {
            if(z+k>=0)
            {
              vector<Boid*> boids = myFlock->getBoids(i+x,j+y,k+z);
              behave(boids);
            }
          }
        }
      }
    }
  }
}

vec3 Boid::ruleSep(vector<Boid*> &boids)
{
  vec3 separation = vec3(0);
  for(Boid *b1:boids)
  {
    vec3 dir = b1->position-position;

    if(this!=b1 && detect(b1))
    {
      if(length(dir)<EPSILON)
        separation += vec3(0,0,0);

      else if(length(dir)<myFlock->sepRadius)
        separation -= normalize(dir)*(10.f/length(dir));
    }

  }

  if(length(separation)>EPSILON)
    separation = normalize(separation);

  return separation;
}

vec3 Boid::ruleAl(vector<Boid*> &boids)
{
  vec3 alignment = vec3(0);
  int velocityNum = 0;
  for(Boid *b1:boids)
  {
    vec3 dir = b1->position-position;

    if(this!=b1 && detect(b1))
    {
      if(length(dir)<myFlock->alignmentRadius)
      {
        alignment += b1->velocity*(10.f/length(dir));
        velocityNum++;
      }
    }
  }

  if(velocityNum>0)
  {
    vec3 avgVel = alignment/(float)velocityNum;
    alignment = avgVel;
  }

  return alignment;
}

vec3 Boid::ruleCohesion(vector<Boid*> &boids)
{
  vec3 cohesion = vec3(0);
  int cohesionNum = 0;
  for(Boid *b1:boids)
  {
    vec3 dir = b1->position-position;

    if(this!=b1 && detect(b1))
    {
      if(length(dir) < myFlock->cohesionRadius)
      {
        cohesion += b1->position;
        cohesionNum++;
      }
    }
  }

  if(cohesionNum>0)
  {
    cohesion = cohesion/(float)cohesionNum;
    cohesion = normalize(cohesion-position);
  }

  return cohesion;
}

vec3 Boid::ruleHerd()
{
  vec3 dir = myFlock->herdPoint - position;
  if(length(dir)>0)
    dir = normalize(dir);
  return dir;
}

vec3 Boid::ruleCounterHerd()
{
  vec3 dir = position-myFlock->herdPoint;
  return dir;
}

void Boid::behave(vector<Boid*> &boids)
{
  vec3 separation = ruleSep(boids);
  vec3 alignment = ruleAl(boids);
  vec3 cohesion = ruleCohesion(boids);

  vec3 herd = vec3(0);
  if(myFlock->herding)
    herd = ruleHerd();
  else if(myFlock->cHerding)
    herd = ruleCounterHerd()*100.f;

  netAcceleration +=  myFlock->sepCoeff*separation +
                      myFlock->alignmentCoeff*alignment +
                      myFlock->cohesionCoeff*cohesion +
                      herd;
}

void Boid::calculateMovement(float t)
{
  //behave();
  hashBehave();
  vec3 acc = netAcceleration;
  float dt = t-prevT;

  if(length(acc)>myFlock->maxAcceleration)
    acc = normalize(acc)*myFlock->maxAcceleration;
  if(length(velocity)>myFlock->maxVelocity)
    velocity = normalize(velocity)*myFlock->maxVelocity;
  if(length(position)>myFlock->radius)
    acc = -normalize(position);

  nextPos = position + velocity*dt +0.5f*acc*dt*dt;
  nextVel = velocity + (0.5f)*(acc+prevAcceleration);

  prevT = t;
  prevAcceleration = acc;
  netAcceleration = vec3(0);
}


void Boid::update()
{
  velocity = nextVel;
  position = nextPos;
}

mat4 Boid::getModelMatrix()
{
  mat4 trans = translate(position);
  vec3 f = normalize(forward);

  vec3 normal = vec3(1);
  vec3 v = normalize(velocity);
  if(length(v)>0 && abs(dot(v,f))<1)
    normal = cross(f, v);


  mat4 rot = rotate(acos(dot(v,f)), normal);

  return trans*rot;
}

Boid::Boid():Boid(vec3(0), vec3(0,0,0),vec3(0)){}

Boid::Boid(vec3 p):Boid(p, vec3(0,0,0), vec3(0)){}

Boid::Boid(vec3 p, vec3 v):Boid(p, v, vec3(0)){}

Boid::Boid(vec3 p, vec3 v, vec3 f)
{
  position = p;
  velocity = v;
  netAcceleration = f;
}

/*
void Boid::behave(vector<Boid*> &boids)
{
  vec3 separation = ruleSep(boids);
  vec3 alignment = ruleAl(boids);
  vec3 cohesion = ruleCohesion(boids);
  vec3 
  /*int cohesionNum = 0;
  int velocityNum = 0;
  for(Boid *b1:boids)
  {
    vec3 dir = b1->position-position;

    if(this!=b1 && detect(b1))
    {
      if(length(dir)<EPSILON)
        separation += vec3(0,0,0);

      else if(length(dir)<myFlock->sepRadius)
        separation -= normalize(dir)*(10.f/length(dir));

      if(length(dir)<myFlock->alignmentRadius)
      {
        alignment += b1->velocity*(10.f/length(dir));
        velocityNum++;
      }

      if(length(dir) < myFlock->cohesionRadius)
      {
        cohesion += b1->position;
        cohesionNum++;
      }
    }
  }

  if(length(separation)>EPSILON)
    separation = normalize(separation);

  if(cohesionNum>0)
  {
    cohesion = cohesion/(float)cohesionNum;
    cohesion = normalize(cohesion-position);
  }

  if(velocityNum>0)
  {
    vec3 avgVel = alignment/(float)velocityNum;
    alignment = avgVel;
  }

  netAcceleration +=  myFlock->sepCoeff*separation +
                      myFlock->alignmentCoeff*alignment +
                      myFlock->cohesionCoeff*cohesion;
}
*/