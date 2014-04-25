#pragma once
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <vector>


#include "libs\glew\glew.h"
#include "libs\glfw\glfw3.h"
#include "libs\glm\glm.hpp"
#include "libs\glm\gtx\transform.hpp"

using namespace glm;
using namespace std;

bool LoadModel(const char* filename);

int GetStrLen(const char* str);
char* FindStr(char* array, const char* str);
char* GetStrSeg(const char* array, int elements_to_copy);

class Model
{
public:
	Model(const char* link,
		GLfloat poz_x, GLfloat poz_y, GLfloat poz_z,
		GLfloat size_x, GLfloat size_y, GLfloat size_z);

	bool Load(const char* link);
	void Draw(mat4 mvp);
	void Run(GLfloat poz_x, GLfloat poz_y, GLfloat poz_z);
	void Size(GLfloat size_x, GLfloat size_y, GLfloat size_z);


	~Model();
private:
	mat4 Matrix;

	GLuint vbo[4];
	GLuint triangles_number;

	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLfloat s_x;
	GLfloat s_y;
	GLfloat s_z;
};