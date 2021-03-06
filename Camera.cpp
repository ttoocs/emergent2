//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
*	Author:	Camilo Talero
*
*
*	Version:	Template
*
*	Implementation of the camera header. Defines the behaviour for a generic camera.
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Includes and macros
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Camera.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//========================================================================================
/*
*	Camera Functions:
*/
//========================================================================================

/*
*	Parameter constructor
*/
Camera::Camera(mat3 frame, vec3 pos, float w, float h)
{
	side = normalize(frame[0]);
	forward = normalize(frame[1]);
	up = normalize(frame[2]);

	position = pos;
	fov = 45;
	width = w;
	height = h;
	zNear = 0.01;
	zFar = 15000;
}

/*
*	Default constructor
*/
Camera::Camera()
{
	side = vec3(1,0,0);
	forward = vec3(0,1,0);
	up = vec3(0,0,1);
	position = vec3(0,0,0);

	width = 1980;
	height = 1024;

	fov = 45;
	zNear = 0.01;
	zFar = 2000;
}


/*
*	Destructor
*/
Camera::~Camera()
{

}

vec3 Camera::getPosition()
{
	return position;
}
/*
*	Get the view matrix of the camera
*/
mat4 Camera::getViewMatrix()
{
	return lookAt(position, position+forward, up);
}

/*
*	Get the perspective matrix of the camera
*/
mat4 Camera::getPerspectiveMatrix()
{
	return perspective(fov, width/height, zNear, zFar);
}

/*
*	Orient the camera
*/
void Camera::setLookDirection(vec3 v)
{
	forward = normalize(v);
	side = cross(forward, up);
}

/*
*	Move the camera by an offset
*/
void Camera::move(vec3 v)
{
	position += v;
}

/*
*	Place the camera at specified position
*/
void Camera::setPosition(vec3 p)
{
	position = p;
}

/*
*	Orientation functions. Used to change the orientation of the camera around it's axis
*/
void Camera::turnH(float angle)
{
	mat4 rotation;
	rotation = rotate(rotation, angle, up);
	vec4 newForward = vec4(forward, 1);

	newForward = rotation*newForward;

	forward = vec3(newForward);
	forward = normalize(forward);
	side = cross(forward, up);
}

void Camera::turnV(float angle)
{
	mat4 rotation;
	if(length(side)>=1)
	{
		rotation = rotate(rotation, angle, side);

		vec4 newForward = vec4(forward, 1);
		vec4 newUp = vec4(up, 1);

		newForward = rotation*newForward;
		newUp = rotation*newUp;

		forward = vec3(newForward);
		up = vec3(newUp);
	}
}

void Camera::incline(float angle)
{
	mat4 rotation;
	rotation = rotate(rotation, angle, forward);
	vec4 newUp = vec4(up, 1);

	newUp = rotation*newUp;

	up = vec3(newUp);
	up = normalize(up);
	side = cross(forward, up);
}

void Camera::resetView()
{
	side = vec3(1,0,0);
	forward = vec3(0,1,0);
	up = vec3(0,0,1);
}

void Camera::resetCamera()
{
	side = vec3(1,0,0);
	forward = vec3(0,1,0);
	up = vec3(0,0,1);
	position = vec3(0,0,0);

	width = 1980;
	height = 1024;

	fov = 45;
	zNear = 0.01;
	zFar = 2000;
}
//########################################################################################
