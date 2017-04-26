#include "Boid.h"
#include <thread>

using namespace std;

vector<CollisionObject*>  Flock::objects;

Flock::Flock(int boidNum)
{
  srand(time(NULL));
  //srand(42);
  boids.clear();
  for(int i=0; i<boidNum; i++)
  {
    int range = radius;
    float x = float(rand()%range- range/2);
    float y = float(rand()%range- range/2);
    float z = float(rand()%range- range/2);

    float v_x = float(rand()%range- range/2);
    float v_y = float(rand()%range- range/2);
    float v_z = float(rand()%range- range/2);
    Boid *b = new Boid(vec3(x,y,z), normalize(vec3(v_x, v_y, v_z)));
    b->myFlock=this;
    boids.push_back(b);
  }
}

#define LP1 961816649
#define LP2 961893937
#define LP3 962228011
void Flock::hash(Boid* b)
{
  if(hashTable.size() == 0)
    hashTable.resize(boids.size()*2);

  uint hashIndex = b->x*LP1+ b->y*LP2 + b->z*LP3;
  hashIndex = hashIndex%hashTable.size();
  uint originalHI = hashIndex;

  while(hashTable[hashIndex].size()>0 &&
    (hashTable[hashIndex][0]->x!=b->x ||
    hashTable[hashIndex][0]->y!=b->y ||
    hashTable[hashIndex][0]->z!=b->z))
  {
    hashIndex = (hashIndex+1)%(hashTable.size());
    if(hashIndex == originalHI)
      return;
  }

  hashTable[hashIndex].push_back(b);
}


vector<Boid*> Flock::getBoids(int i, int j, int k)
{
  uint hashIndex = i*LP1+ j*LP2 + k*LP3;
  hashIndex = hashIndex%hashTable.size();
  uint originalHI = hashIndex;
  while(hashTable[hashIndex].size()>0 &&
    (hashTable[hashIndex][0]->x!=i ||
    hashTable[hashIndex][0]->y!=j ||
    hashTable[hashIndex][0]->z!=k))
  {
    hashIndex= (hashIndex+1)%(hashTable.size());
    if(hashIndex==originalHI)
      return vector<Boid*>();
  }

  return hashTable[hashIndex];
}

void Flock::allocate()
{
  for(Boid *b: boids)
  {
    int x = (b->position.x+radius)/cohesionRadius;
    int y = (b->position.y+radius)/cohesionRadius;
    int z = (b->position.z+radius)/cohesionRadius;

    b->x=x;
    b->y=y;
    b->z=z;

    hash(b);
  }

  /*int c =0;
  for(vector<Boid*> v: hashTable)
  {
    cout << c << ", "<< v.size() << endl;
        c++;
  }*/
}

void semiMove(vector<Boid*> *boids, uint start, uint end, float t)
{
  vector<Boid *> localBoids = *boids;
  for(uint i=start; i<end; i++)
  {
    Boid *b = localBoids[i];
    b->calculateMovement(t);
  }

  //setpriority(PRIO_PROCESS, 0, -20);

  /*cout << "Running with priority: " << getpriority(PRIO_PROCESS, 0) << endl;
  cout <<"User: " << getuid() << endl;*/
}

void semiUpdate(vector<Boid*> *boids, uint start, uint end)
{
  vector<Boid *> localBoids = *boids;
  for(uint i=start; i<end; i++)
  {
    Boid *b = localBoids[i];
    b->update();
  }
}

void Flock::update(float t)
{
  hashTable.clear();
  allocate();

  unsigned int nthreads = thread::hardware_concurrency();
  vector<thread> threads;

  uint start = 0;
  uint end = 0;
  for(int i=0; i<(int)nthreads; i++)
  {
    start = boids.size()*i/nthreads;
    end = boids.size()*(i+1)/nthreads;
    if(end>boids.size())
      end = boids.size();
    threads.push_back( thread(&semiMove, &boids, start, end, t));
  }

  for (auto& th : threads)
    th.join();

  //allocate(boids);
  threads.clear();

  for(int i=0; i<(int)nthreads; i++)
  {
    start = boids.size()*i/nthreads;
    end = boids.size()*(i+1)/nthreads;
    if(end>boids.size())
      end = boids.size();
    threads.push_back( thread(&semiUpdate, &boids, start, end));
  }

  for (auto& th : threads)
    th.join();
}

Flock::Flock(string fileName)
{
  ifstream inFile(fileName, ios::in);
  string line;
  int boidNum = 0;
  while(getline(inFile, line))
  {
    if(line != "" && line[0]!='#')
    {
      string word;
      istringstream iss(line);
      iss >> word;
      if(word == "BoidNum:")
      {
        iss >> word;
        boidNum = stoi(word);
      }

      else if(word == "maxAcceleration:")
      {
        iss >> word;
        maxAcceleration = stof(word);
      }

      else if(word == "maxVelocity:")
      {
        iss >> word;
        maxVelocity = stof(word);
      }

      else if(word == "fov:")
      {
        iss >> word;
        fov = stof(word);
      }

      else if(word == "sepRadius:")
      {
        iss >> word;
        sepRadius = stof(word);
      }

      else if(word == "alignmentRadius:")
      {
        iss >> word;
        alignmentRadius = stof(word);
      }

      else if(word == "cohesionRadius:")
      {
        iss >> word;
        cohesionRadius = stof(word);
      }

      else if(word == "sepCoeff:")
      {
        iss >> word;
        sepCoeff = stof(word);
      }

      else if(word == "alignmentCoeff:")
      {
        iss >> word;
        alignmentCoeff = stof(word);
      }

      else if(word == "cohesionCoeff:")
      {
        iss >> word;
        cohesionCoeff = stof(word);
      }

      else if(word == "radius:")
      {
        iss >> word;
        radius = stof(word);
      }

      else if(word == "center:")
      {
        iss >> word;
        float x = stof(word);

        iss >> word;
        float y = stof(word);

        iss >> word;
        float z = stof(word);

        center = vec3(x,y,z);
      }

      else if(word == "color:"){
        iss >> word;
        float r = stof(word);

        iss >> word;
        float g = stof(word);

        iss >> word;
        float b = stof(word);

        color = vec3(r,g,b);
      }

      else
        cerr << "Unrecongized line: " << line << endl;
    }
  }

  inFile.close();

  srand(time(NULL));
  boids.clear();
  for(int i=0; i<boidNum; i++)
  {
    int range = radius/2;
    float x = float(rand()%range- range/2);
    float y = float(rand()%range- range/2);
    float z = float(rand()%range- range/2);

    float v_x = float(rand()%range- range/2);
    float v_y = float(rand()%range- range/2);
    float v_z = float(rand()%range- range/2);
    Boid *b = new Boid(vec3(x,y,z), normalize(vec3(v_x, v_y, v_z)));
    b->myFlock=this;
    boids.push_back(b);
  }
}

//vector<vector<vector<vector<Boid*>>>>  Flock::grid;

/*void subdivide(vector<Boid*> &boids)
{
  vec3 max = vec3(radius);
  vec3 min = vec3(-radius);

  int xdiv = (max.x-min.x)/cohesionRadius+1;
  int ydiv = (max.y-min.y)/cohesionRadius+1;
  int zdiv = (max.z-min.z)/cohesionRadius+1;

  grid.resize(xdiv);
  for(int i=0; i<xdiv; i++)
  {
    grid[i].resize(ydiv);
    for(int j=0; j<ydiv; j++)
    {
      grid[i][j].resize(zdiv);
    }
  }
}*/
