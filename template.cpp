//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
*	Author:	Camilo Talero, Scott Saunders
*
* Class:  Cpsc565
*
*
*	References:
*	https://open.gl
*	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
*	http://www.glfw.org/docs/latest/
*
*	Note: Based on the Boiler Plate written by Dr. Sonny Chan, University of Calgary,
*		Alberta Canada.
*
*
* Dev-notes: X,Y,Z -> left/right,back/forward,up/down in the default camera frame.
*
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Includes and macros
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define GLEW_DYNAMIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
#include <ft2build.h>
#include <ctime>
#include <thread>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include FT_FREETYPE_H

#include "Camera.h"
//#include "CustomOperators.h"
#include "ObjParser.h"
#include "Boid.h"

using namespace std;
using namespace glm;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Structure definitions
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct Shader
{
	string fileName;
	GLuint shaderID;
	GLuint type;
	GLuint program;
};

struct Geometry
{
	GLuint vertexArray;

	GLuint vertexBuffer;
	GLuint elmentBuffer;
	GLuint normalsBuffer;
	GLuint uvsBuffer;

	vector<vec3> vertices;
	vector<uint> indices;
	vector<vec3> normals;
	vector<vec2> uvs;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Global Values
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Camera cam;

//Flock *flock;		//Switching away for multi-flock!

#define flockWrapperSet(X,Y) if(selectedFlock != NULL){selectedFlock->X = Y;} else { for(Flock * f: flocks) {f->X = Y;} }

vector<Flock *> flocks;
Flock * selectedFlock;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//========================================================================================
/*
*	List of function headers:
*/
//========================================================================================

void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void callBackInit(GLFWwindow* window);
void loadGeometryArrays(GLuint program, Geometry &g);
void setDrawingMode(int mode, GLuint program);
void render(Geometry &g, GLenum drawType);
void compileShader(GLuint &shader, string &filepath, GLenum shaderType);
void initDefaultShaders(vector<Shader> &shaders);
void initDefaultProgram(vector<GLuint> &programs, vector<Shader> &shaders);
void createShader(Shader &s, string file, GLenum type);
void deleteShader(Shader &s);
void createGeometry(Geometry &g, vector<vec3> vertices, vector<uint> indices);
void createGeometry(Geometry &g);
void deleteGeometry(Geometry &g);

void initFlocks(int numFlocks);
void initFlocks();

GLFWwindow* createWindow();

string loadSourceFile(string &filepath);
string mat4ToString(mat4 m);

GLuint createShadingProgram(GLuint vertexShader, GLuint fragmentShader);
int loadViewProjMatrix(Camera &c, GLuint &program);
int loadColor(vec4 color, GLuint program);
int loadCamera(vec3 cameraPos, GLuint program);
int loadModelMatrix(mat4 model, GLuint &program);
int openGLerror();

double calculateFPS(double prevTime, double currentTime);
//########################################################################################

//--------------------------------------------------------------------------------------\\
//**************************************************************************************\\
git@github.com:ttoocs/emergent2.git
//**************************************************************************************\\
//--------------------------------------------------------------------------------------\\

int main(int argc, char **argv)
{
	/*FT_Library ft;

	if(FT_Init_FreeType(&ft)) {
	  fprintf(stderr, "Could not init freetype library\n");
	  return 1;
	}

	FT_Face face;

	if(FT_New_Face(ft, "Fonts/OptimusPrinceps.ttf", 0, &face)) {
	  fprintf(stderr, "Could not open font\n");
	  return 1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
  	fprintf(stderr, "Could not load character 'X'\n");
  	return 1;
	}*/

	GLFWwindow* window = createWindow();

	callBackInit(window);

	// glew initilization, this is so that the program is crossplatform,
	// also things won't work without it
	glewExperimental = GL_TRUE;
	glewInit(); glGetError();
	//An error will always be thrown when initializing glew.
	//It can be safely discarded so we call glGetError() to delete it and move on.

//Initalize openGL structs.
//**********************************************************************************
	vector<GLuint> programs;
	vector<Shader> shaders;
	vector<Geometry> shapes(3);

	initDefaultShaders(shaders);
	initDefaultProgram(programs, shaders);

	createGeometry(shapes[0]);
	createGeometry(shapes[1]);
	createGeometry(shapes[2]);

  //Other openGL stuff.

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glPointSize(10.f);
	float t=0;

	errno = 0;
	setpriority(PRIO_PROCESS, 0, -20);
//**********************************************************************************

  //priority out
  cout << "Running with priority: " << getpriority(PRIO_PROCESS, 0) << endl;
  cout <<"User: " << getuid() << endl;

  //The flocks:

	loadObjFile("Models/PyramidBoid.obj", shapes[0].vertices, shapes[0].normals, shapes[0].uvs, shapes[0].indices);
	initFlocks(5);	//Make n flocks.
	selectedFlock = flocks[0];

	loadGeometryArrays(programs[0], shapes[0]);
	loadColor(vec4(1,0.3,0,1), programs[0]);
	setDrawingMode(1,programs[0]);

  //The "ground"
	float side = 10000;
	float down = -1000;
	shapes[1].vertices = {vec3(side, side, down), vec3(side, -side, down),
		vec3(-side, -side, down), vec3(-side, side, down)};
	shapes[1].normals = {vec3(0,0,1), vec3(0,0,1), vec3(0,0,1), vec3(0,0,1)};

	shapes[1].indices = {0,1,2,3,0};
	loadGeometryArrays(programs[0], shapes[1]);

	cam = *(new Camera(mat3(1), vec3(0,-200,0), width, height));

  //The center sphere. (well, actually slightly off center..)

	vector<Sphere*> Spheres;
	#define ArangeSX 250
	#define ArangeSY 250
	#define ArangeSZ 250
	#define ArangeSRmn 1
	#define ArangeSRmx 20
	for(int i=0; i < 7 ; i++){

		Sphere *s = new Sphere(vec3((float) ((rand() % ArangeSX*2 )- ArangeSX),
			(float) ((rand() % ArangeSY*2 )- ArangeSY),
			(float) ((rand() % ArangeSZ*2 )- ArangeSZ)),
			(float) ((rand() % ArangeSRmx) + (ArangeSRmn)) );

			//Sphere *s = new Sphere(vec3(0,0,0),5);
		Spheres.push_back(s);
		Flock::objects.push_back(s);
	}

	while (!glfwWindowShouldClose(window))
	{
		if(loadViewProjMatrix(cam, programs[0])!=0)
			return 1;

	//	float temp = t;
		t = glfwGetTime();
		// cout << calculateFPS(temp, t) << endl;*/
		//t+=0.005;
		glClearColor(0.f, 0.8f, 1.f, 1.0f); //blue-ish sky.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setDrawingMode(1,programs[0]);
		glUseProgram(programs[0]);
		for(Flock * f : flocks){
			f->update(t);
			loadColor(vec4(f->color,1), programs[0]); //This should be a property of the flock itself
			for(Boid *b: f->boids)
			{
				loadModelMatrix(b->getModelMatrix(), programs[0]);
				render(shapes[0], GL_TRIANGLE_STRIP);
				//render(shapes[1], GL_POINTS);
			}
		}

		//setDrawingMode(0,programs[0]);
		//Draw ground:
		loadColor(vec4(0,0.3,0,1), programs[0]);
		loadModelMatrix(mat4(1), programs[0]);
		render(shapes[1], GL_TRIANGLE_STRIP);


		//Draw spheres
		loadColor(vec4(0,0,1,1), programs[0]);

		for(Sphere * s: Spheres){
			s->getGeometry(shapes[2].vertices, shapes[2].normals, shapes[2].indices);
			loadGeometryArrays(programs[0], shapes[2]);
			render(shapes[2], GL_TRIANGLE_STRIP);
		}


		//cam.setPosition(flock->boids[0]->position + vec3(0,-200,0));
		//cam.setLookDirection(flock->boids[0]->velocity);

		loadCamera(cam.getPosition(), programs[0]);

	  glfwPollEvents();
	  glfwSwapBuffers(window);
	}
	//Cleanup
	for(Shader s: shaders)
		deleteShader(s);
	for(GLuint p: programs)
		glDeleteProgram(p);
	for(Geometry g: shapes)
		deleteGeometry(g);

	glfwDestroyWindow(window);
	glfwTerminate();
}
//**************************************************************************************\\

void initFlocks(int numFlocks){
    while(flocks.size() > 0){
        int i = flocks.size()-1;
        Flock * f = flocks[i];
        f->boids.clear();
        delete(f);
        flocks.pop_back();
        glfwSetTime(0);
    }

    for(int i =0; i < numFlocks; i++){
				string filename;
				filename = "FlockInfo" + to_string(i) + ".txt";
				//filename = "FlockInfo.txt"
        Flock * flock = new Flock(filename);
        flocks.push_back(flock);
    }

    selectedFlock = NULL;
}

void initFlocks(){
    int numFlocks = flocks.size();
    initFlocks(numFlocks);
}

//========================================================================================
/*
*	Rendering Functions:
*/
//========================================================================================


//This isn't setting a mode, its setting a uniform locations.
void setDrawingMode(int mode, GLuint program)
{
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "drawMode");
	if(loc == GL_INVALID_VALUE || loc==GL_INVALID_OPERATION)
	{
		cerr << "Error returned when trying to find uniform location."
			<< "\nuniform: drawMode"
			<< "Error num: " << loc
			<< endl;
	}
	glUniform1i(loc, mode);
}

//Need more versions of this:
void loadGeometryArrays(GLuint program, Geometry &g)
{
	glUseProgram(program);

	glBindVertexArray(g.vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, g.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, g.vertices.size()*sizeof(vec3),
		g.vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

	if(g.normals.size()>0)
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, g.normalsBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vec3), (void*)0);
		glBufferData(GL_ARRAY_BUFFER, g.normals.size()*sizeof(vec3),
			g.normals.data(), GL_STATIC_DRAW);
	}

	if(g.indices.size()>0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.elmentBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, g.indices.size()*sizeof(uint),
			g.indices.data(), GL_STATIC_DRAW);
	}
}

void render(Geometry &g, GLenum drawType)
{
	glBindVertexArray(g.vertexArray);

	if(g.indices.size()>0)
		glDrawElements(drawType, g.indices.size(), GL_UNSIGNED_INT, (void*)0);

	else
		glDrawArrays(drawType, 0, g.vertices.size());
}

int loadModelMatrix(mat4 model, GLuint &program)
{
	GLint loc = glGetUniformLocation(program, "model");
	if(loc == GL_INVALID_VALUE || loc==GL_INVALID_OPERATION)
	{
		cerr << "Error returned when trying to find uniform location."
			<< "\nuniform: model"
			<< "Error num: " << loc
			<< endl;
		return -1;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(model));

	return 0;
}

int loadViewProjMatrix(Camera &c, GLuint &program)
{
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "view");
	if(loc == GL_INVALID_VALUE || loc==GL_INVALID_OPERATION)
	{
		cerr << "Error returned when trying to find uniform location."
			<< "\nuniform: view"
			<< "Error num: " << loc
			<< endl;
		return -1;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(c.getViewMatrix()));

	loc = glGetUniformLocation(program, "proj");
	if(loc == GL_INVALID_VALUE || loc==GL_INVALID_OPERATION)
	{

		cerr << "Error returned when trying to find uniform location."
			<< "\nuniform: proj"
			<< "Error num: " << loc
			<< endl;
		return -1;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(c.getPerspectiveMatrix()));

	return 0;
}

int loadColor(vec4 color, GLuint program)
{
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "color");
	/*if (loc == -1)
	{
		cerr << "Uniform: error loading \"color\"." << endl;
		return -1;
	}*/
	glUniform4f(loc, color[0], color[1], color[2], color[3]);

	return 1;
}

int loadCamera(vec3 cameraPos, GLuint program)
{
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "cameraPos");
	if (loc == -1)
	{
		cerr << "Uniform: error loading \"cameraPos\"." << endl;
		return -1;
	}
	glUniform3f(loc, cameraPos[0], cameraPos[1], cameraPos[2]);

	return 1;
}
//########################################################################################

//========================================================================================
/*
*	Shader Functions:
*/
//========================================================================================
void createShader(Shader &s, string file, GLenum type)
{
	s.fileName = file;
	compileShader(s.shaderID, file, type);
	s.type = GL_VERTEX_SHADER;
	s.program = 0;
}

void deleteShader(Shader &s)
{
	glUseProgram(0);
	glDeleteShader(s.shaderID);
	s.program = 0;
}

GLuint createShadingProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	return shaderProgram;
}

void compileShader(GLuint &shader, string &filename, GLenum shaderType)
{
	string source = loadSourceFile(filename);
	const GLchar* s_ptr = source.c_str();

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &s_ptr, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status!=GL_TRUE)
	{
		cout << "Shader compilation error. Could not compile: "
		<< filename << "\nShader type: "
		<< shaderType
		<<endl;

		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		string log(length, ' ');
		glGetShaderInfoLog(shader, log.length(), &length, &log[0]);

		cerr<< endl << source <<endl;
		cerr << endl << log <<endl;
	}
}

string loadSourceFile(string &filepath)
{
	string source;

	ifstream input(filepath.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}

	else {
		cerr << "ERROR: Could not load shader source from file: "
			<< filepath << endl;
	}

	return source;
}
//########################################################################################


//========================================================================================
/*
*	Geometry Functions:
*/
//========================================================================================
void createGeometry(Geometry &g, vector<vec3> vertices, vector<uint> indices)
{
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &(g.vertexBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, g.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3),
		vertices.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &(g.elmentBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.elmentBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices.size()*sizeof(uint),
		indices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(1);
	glGenBuffers(1, &g.normalsBuffer);

	glGenVertexArrays(1, &(g.vertexArray));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	g.vertices=vertices;
	g.indices=indices;
}

void createGeometry(Geometry &g)
{
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &(g.vertexBuffer));

	glGenBuffers(1, &(g.elmentBuffer));

	glEnableVertexAttribArray(1);
	glGenBuffers(1, &g.normalsBuffer);

	glGenVertexArrays(1, &(g.vertexArray));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void deleteGeometry(Geometry &g)
{
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &(g.vertexArray));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &(g.vertexBuffer));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &(g.elmentBuffer));
}
//########################################################################################

//========================================================================================
/*
*	Error checking and debugging functions:
*/
//========================================================================================

int openGLerror()
{
	GLenum errorNum = glGetError();
	switch(errorNum)
	{
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
			cerr << "An unacceptable value is specified for an enumerated argument."
				<< endl;
			break;
		case GL_INVALID_VALUE:
			cerr << "A numeric argument is out of range."
				<< endl;
			break;
		case GL_INVALID_OPERATION:
			cerr << "The specified operation is not allowed in the current state."
				<< endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cerr << "The framebuffer object is not complete."
				<< endl;
			break;
		case GL_OUT_OF_MEMORY:
			cerr << "There is not enough memory left to execute the command."
				<< endl;
			break;
		default:
			cerr<< "Undefined error, you messed up big time now."
				<< endl;
	}
	return errorNum;
}

/*
*	Make a string representing the values of a mat4.
*/
string mat4ToString(mat4 m)
{
	string s = "";
	s += "\n{";
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			s += to_string(m[i][j]) + ", ";
		}
		if(i!=3)
			s += "\n";
	}
	s += "}\n";
	return s;
}

double calculateFPS(double prevTime, double currentTime)
{
	double elapsedTime = currentTime - prevTime;
	return 1.f/elapsedTime;
}
//########################################################################################

//========================================================================================
/*
*	Initialization:
*/
//========================================================================================
//Initialize GLFW callBack Functions
void callBackInit(GLFWwindow* window)
{
	//Set GLFW callback functions
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

}

//Initialize window
//WARNING: Not sure if this is correct, if bugs appear suspect this function!
GLFWwindow* createWindow()
{
	//Initialize GLFW
	if (!glfwInit())
	{
		cerr<< "Failed to initialize GLFW.\nTerminating program." << endl;
		return NULL;
	}

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height-40,
		"BOIDS", NULL, NULL);
	if (!window)
	{

		cerr<< "Failed to glfwCreateWindow.\nTerminating program." << endl;
		return NULL;
	}
	//glfwMaximizeWindow(window);
	glfwMakeContextCurrent(window);

	return window;
}

/*
*	InitDefaultProgram and InitDefaultShader are examples on how to initialize
*	the OpenGL pipeline. It is recommended to re-implement these.
*/
void initDefaultProgram(vector<GLuint> &programs, vector<Shader> &shaders)
{
	programs.push_back(glCreateProgram());
	glAttachShader(programs[0], shaders[0].shaderID);
	glAttachShader(programs[0], shaders[1].shaderID);

	glLinkProgram(programs[0]);
	glUseProgram(programs[0]);

	shaders[0].program=programs[0];
	shaders[1].program=programs[0];
}

void initDefaultShaders(vector<Shader> &shaders)
{
	string str1 = "Shaders/VertexShader.glsl";
	Shader s1;
	shaders.push_back(s1);

	createShader(shaders[0], str1, GL_VERTEX_SHADER);

	string str2 = "Shaders/FragmentShader.glsl";
	Shader s2;
	shaders.push_back(s2);

	createShader(shaders[1], str2, GL_FRAGMENT_SHADER);
}
//########################################################################################

//========================================================================================
/*
*	GLFW helper functions:
*/
//========================================================================================

/*vec2 mapCursorToWindow(vec2 pos, GLFWwindow *window)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	pos.x = (2*xpos-width)/width;
	pos.y = -(2*(ypos)-height)/(height);

	return pos;
}*/

int cursorSelectNode(GLFWwindow *window)
{
	/*double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	mat4 view= cam.getViewMatrix();
	mat4 proj= cam.getPerspectiveMatrix();

	uint count = 0;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	vec3 screenPos;
	for(Node *node : graph->nodes)
	{
		screenPos = project(node->position, view, proj, vec4(0.f,0.f,(float)width, (float)height));
		float depth = screenPos.z;
		vec2 v = vec2(xpos, height-ypos);
		vec3 projCursor = unProject(vec3(v.x,v.y,depth), view, proj, vec4(0.f,0.f,(float)width, (float)height));

		cout << "Center: " << screenPos << endl;
		cout << "Cursor: " << v << endl;

		if(length(projCursor-node->position) < node->size)
			break;

		count++;
	}
	cout << endl;
	if(count < graph->nodes.size())
		return count;
	else
		return -1;*/
		return 1;
}
//########################################################################################

//========================================================================================
/*
*	GLFW callback functions:
*/
//========================================================================================

void error_callback(int error, const char* description)
{
    cout << "Error: " << description << endl;
}


void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	int b1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int b2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (b1 == GLFW_PRESS || b2 == GLFW_PRESS)
	{

		uint c = 0;
		vec3 avgPos = vec3(0);

		if(selectedFlock != NULL){
			uint offset  = selectedFlock->boids.size()/20;
			for(uint i=0; i<selectedFlock->boids.size(); i+=offset)
			{
				avgPos += selectedFlock->boids[i]->position;
				c++;
			}
		}else{  //No selected flock:
      for(Flock * f : flocks){
        uint offset  = f->boids.size()/20;
        for(uint i=0; i< f->boids.size(); i+=offset)
        {
          avgPos += f->boids[i]->position;
          c++;
        }
      }
    }

		mat4 view= cam.getViewMatrix();
		mat4 proj= cam.getPerspectiveMatrix();

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		avgPos *= (1.f/(float)c);

		vec2 v = vec2(xpos, height-ypos);
		float depth = project(avgPos, view, proj, vec4(0.f,0.f,(float)width, (float)height)).z;
		vec3 projCursor = unProject(vec3(v.x,v.y,depth), view, proj, vec4(0.f,0.f,(float)width, (float)height));


		flockWrapperSet(herdPoint,projCursor);
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS)
	{
		flockWrapperSet(herding,true);
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT && action==GLFW_RELEASE)
	{
		flockWrapperSet(herding,false);
	}

	if(button == GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS)
	{
		flockWrapperSet(cHerding,true);
	}
	if(button == GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_RELEASE)
	{
		flockWrapperSet(cHerding,false);
	}
}

#define CAM_SPEED 1.f
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    else if(key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
    	//Get the primary monitor and the monitor attached to the current window
    	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    	GLFWmonitor* monitor = glfwGetWindowMonitor(window);

    	//If no monitor is attached to the window attach the primary monitor
    	//This makes the window full screen
    	if(monitor==NULL)
    		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0,
    			mode->width, mode->height, mode->refreshRate);

    	//If a monitor is attached, detach it and set the dimensions to that of
    	//The primary's monitor resolution. This makes the window windowed
    	else
    		glfwSetWindowMonitor(window, NULL, 0, 0,
    			mode->width, mode->height-1, mode->refreshRate);
    			//subtract 1 to prevent it from going into full screen mode

    	glfwMaximizeWindow(window);
    }

    else if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
            initFlocks();
    }

    else if(key == GLFW_KEY_F12 && action == GLFW_PRESS)
    	cout << glfwGetVersionString() << endl;

    else if(key == GLFW_KEY_W)
    	cam.move(vec3(0,CAM_SPEED,0));

    else if(key == GLFW_KEY_S)
    	cam.move(vec3(0,-CAM_SPEED,0));

    else if(key == GLFW_KEY_A)
    	cam.move(vec3(-CAM_SPEED,0,0));

    else if(key == GLFW_KEY_D)
    	cam.move(vec3(CAM_SPEED,0,0));

    else if(key == GLFW_KEY_Q)
    	cam.move(vec3(0,0,-CAM_SPEED));

    else if(key == GLFW_KEY_E)
    	cam.move(vec3(0,0, CAM_SPEED));

    else if(key == GLFW_KEY_KP_6)
    	cam.turnH(radians(-1.f));

    else if(key == GLFW_KEY_KP_4)
    	cam.turnH(radians(1.f));

    else if(key == GLFW_KEY_KP_8)
    	cam.turnV(radians(1.f));

    else if(key == GLFW_KEY_KP_2)
    	cam.turnV(radians(-1.f));

    else if(key == GLFW_KEY_KP_ADD)
    	cam.incline(radians(1.f));

    else if(key == GLFW_KEY_KP_SUBTRACT)
    	cam.incline(radians(-1.f));

    else if(key == GLFW_KEY_KP_MULTIPLY)
    	cam.resetView();

    else if(key == GLFW_KEY_KP_DIVIDE)
    	cam.resetCamera();
}
//########################################################################################
