#pragma once
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "libs\glew\glew.h"
#include "libs\glfw\glfw3.h"
#include "libs\glm\glm.hpp"
#include "libs\glm\gtx\transform.hpp"

using namespace glm;
#define PI 3.14159265358979f

class bilboard
{
public:
	bilboard(GLfloat poz_x, GLfloat poz_y, GLfloat poz_z, GLfloat b_size);
	GLuint getIndyk();
	GLuint useVertex();
	GLuint useColor();
	GLfloat getPozX();
	GLfloat getPozY();
	GLfloat getPozZ();
	mat4 getModelMatrix();
	void b_scale(GLfloat s_pozX, GLfloat s_pozY, GLfloat s_pozZ);
	void b_tranform(GLfloat t_pozX, GLfloat t_pozY, GLfloat t_pozZ);

	~bilboard();
private:
	GLuint indorID;
	GLuint Vertex;
	GLuint Color;

	GLfloat b_pozX;
	GLfloat b_pozY;
	GLfloat b_pozZ;
	mat4 modelMatrix;
	mat4 transform;
	mat4 scaleModel;
};

