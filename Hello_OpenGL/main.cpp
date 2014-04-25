#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "libs\glew\glew.h"
#include "libs\glfw\glfw3.h"
#include "libs\glm\glm.hpp"
#include "libs\glm\gtx\transform.hpp"
#include "bilboard.h"
#include "Model.h"

#pragma comment ( lib, "opengl32.lib")
#pragma comment ( lib, "libs/glew/glew32.lib")
#pragma comment ( lib, "libs/glfw/glfw3.lib")

using namespace glm;
using namespace std;
#define PI 3.14159265358979f

const char* vs =
"#version 330 core\n"
"#extension GL_ARB_explicit_uniform_location : enable\n"
"layout(location=0)in vec3 vertex;"
"layout(location=1)in vec3 color;"
"layout(location=2)in vec2 vertexUV;"
"layout(location=3)in vec3 normal;"
"layout(location=0)uniform mat4 wvp;"
"layout(location=1)uniform vec3 lightDir;"

"out vec3 o_color;"
"out vec2 UV;"
"out float diffuse;"

"void main(){"
"gl_Position = wvp* vec4(vertex,1);"
"o_color = color;"
"UV = vertexUV;"
"diffuse = max(dot(normal, lightDir), 0);"
"}\n";
const char* fs =
"#version 330 core\n"
"in vec3 o_color;"
"in vec2 UV;"
"in float diffuse;"

"out vec3 oo_color;"
"uniform sampler2D myTextureSampler;"
"void main(){"
"oo_color = texture( myTextureSampler, UV ).rgb * diffuse;"
"}\n";


GLuint MakeSquare(GLfloat x, GLfloat y, GLfloat z, GLfloat size);
GLuint loadBMP(const char* imagepatch);
//fps camera
vec3 c_position = vec3(0.5, 0.5, -0.5);
mat4 c_ProjectMatrix;
mat4 c_ViewMatrix;
void computeMatricesFromInputs();

GLFWwindow* win;
int main()
{
	srand(time_t(NULL));
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 4);
	win = glfwCreateWindow(1500, 800, "The Lorem", NULL, NULL);
	glfwMakeContextCurrent(win);
	glEnable(GL_CULL_FACE);
	glewInit();

	glViewport(0, 0, 1500, 800);
	glClearColor(0.005, 0.111, 0.2, 1);


	GLuint ivs = glCreateShader(GL_VERTEX_SHADER);
	GLuint ifs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ivs, 1, (const char**)&vs, 0);// 2 - il tablic z kodami, 4-dl kazdego z kodu(0=na koncu kodu 0)
	glShaderSource(ifs, 1, (const char**)&fs, 0);
	glCompileShader(ivs);
	glCompileShader(ifs);
	GLuint program = glCreateProgram();
	glAttachShader(program, ivs); // attach - dolaczyc
	glAttachShader(program, ifs);
	glLinkProgram(program);
	glUseProgram(program);
	char* log = new char[1024];
	glGetProgramInfoLog(program, 1024, 0, log);
	if (strlen(log)){
		MessageBoxA(0, log, "Shader compile status", 0);
		exit(-1);
	}



	GLfloat* color = new GLfloat[108];
	for (unsigned i = 0; i < 108; i++) color[i] = (rand() % 1000 + 1) / 1000.f;
	GLfloat* UV = new GLfloat[72];
	for (unsigned i = 0; i < 72; i += 6){
		UV[i] = 0.0f;
		UV[i + 1] = 0.0f;

		UV[i + 2] = 0.5f;
		UV[i + 3] = 0.0f;

		UV[i + 4] = 0.5f;
		UV[i + 5] = 0.5f;
	}

	GLuint vbo = MakeSquare(-0.1, -0.1, 0, 0.3f);;// tworzymy 2 identyfikatory
	GLuint cbo;
	glGenBuffers(1, &cbo);// tworzymy dwa buffory dla tych identyfikatorow
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, 108 * 4, color, GL_STATIC_DRAW);// 1 typ tablicy, ost: typ tablicy :I,dynamiczna, statyczna ( brak edycji)
	GLuint uvo;
	glGenBuffers(1, &uvo);
	glBindBuffer(GL_ARRAY_BUFFER, uvo);
	glBufferData(GL_ARRAY_BUFFER, 72 * 4, UV, GL_STATIC_DRAW);
	GLuint texture;
	texture = loadBMP("x.bmp");
	bilboard bilb(0.5f, 0.5f, 0.f, 1.5f);
	GLuint skybox_texture = loadBMP("S_B.bmp");
	Model skybox("de.ply",
		-5, 0, -2,
		0.5, 0.5, 0.5
		);

	delete[]color;
	delete[]UV;


	GLfloat kat = 0.f;
	while (glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(win)){
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		computeMatricesFromInputs();
		mat4 mvp = c_ProjectMatrix *	c_ViewMatrix;// perspektywa min odleglosc obiektu,st.wymiarow okna,max odleglosc
           //widok ,polozenie oka,punkt obserwacji,gora sceny'

		glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);//indeks uniformu, il.macierzy

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // nr slotu, dl.sekwencji, typ tablicy
		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, uvo);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawArrays(GL_TRIANGLES, 0, 36); // typ rysowania, poczatek tablicy, il pkt do narysowania
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);



		GLfloat x = acos((c_position.z - bilb.getPozZ()) / sqrt(pow(c_position.x - bilb.getPozX(), 2) + pow(c_position.z - bilb.getPozZ(), 2)));
		GLfloat y = (c_position.x < bilb.getPozX() ? 360 - (360 * x) / (2 * PI) : (360 * x) / (2 * PI));
		std::cout << y << std::endl;
		mat4 mvp_2 = mvp * rotate(y, vec3(0, 1, 0)) *
			bilb.getModelMatrix();
		glUniformMatrix4fv(0, 1, GL_FALSE, &mvp_2[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, bilb.useVertex());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, bilb.useColor());
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bilb.getIndyk());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);


		glBindTexture(GL_TEXTURE_2D, skybox_texture);
		skybox.Draw(mvp);


		glfwSwapBuffers(win);
	}
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &uvo);
	glDeleteProgram(program);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &skybox_texture);
	glfwDestroyWindow(win);

	glfwTerminate();
	return 0;
}


GLuint MakeSquare(GLfloat x, GLfloat y, GLfloat z, GLfloat size){

	static GLfloat tab[108] = {
		x + size, y, z,
		x, y, z,
		x, y, z - size,

		x, y, z - size,
		x + size, y, z - size,
		x + size, y, z,
		//--- 1

		x, y, z - size,
		x, y, z,
		x, y + size, z,

		x, y + size, z,
		x, y + size, z - size,
		x, y, z - size,
		//--- 2

		x + size, y, z - size,
		x, y, z - size,
		x, y + size, z - size,

		x, y + size, z - size,
		x + size, y + size, z - size,
		x + size, y, z - size,
		//--- 3

		x + size, y + size, z,
		x + size, y, z,
		x + size, y, z - size,

		x + size, y, z - size,
		x + size, y + size, z - size,
		x + size, y + size, z,
		//--- 4

		x, y + size, z,
		x, y, z,
		x + size, y, z,

		x + size, y, z,
		x + size, y + size, z,
		x, y + size, z,
		//--- 5
		x, y + size, z,
		x + size, y + size, z,
		x + size, y + size, z - size,

		x + size, y + size, z - size,
		x, y + size, z - size,
		x, y + size, z
		//--- 6
	};
	GLuint v;
	glGenBuffers(1, &v);

	glBindBuffer(GL_ARRAY_BUFFER, v);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tab), tab, GL_STATIC_DRAW);


	return v;
}
GLuint loadBMP(const char* imagepatch){
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;

	unsigned char* data;

	FILE* file;
	errno_t err = fopen_s(&file, imagepatch, "r");

	if (err != 0){ printf("Image could not be opend\n"); return 0; }

	if (fread(header, 1, 54, file) != 54){
		printf("Not a corrent BMP file\n");
		return false;
	}
	if (header[0] != 'B' || header[1] != 'M'){
		printf("Not a correct BMP file\n");
		return 0;
	}


	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0) imageSize = width*height * 3;
	if (dataPos == 0) dataPos = 54;


	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);

	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete[] data;
	err = fclose(file);

	return textureID;
}
float c_angleX = 0;
float c_angleY = 0;
float c_FoV = 45.f;
float speed = 3.f;
float c_mouseSpeed = 0.005f;
void computeMatricesFromInputs(){
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	double xpos, ypos;
	glfwGetCursorPos(win, &xpos, &ypos);
	glfwSetCursorPos(win, 1500 / 2, 800 / 2);

	c_angleX += c_mouseSpeed * float(1500 / 2 - xpos);
	c_angleY += c_mouseSpeed * float(800 / 2 - ypos);

	if (c_angleY > PI / 2 || c_angleY < -PI / 2){
		if (c_angleY > PI / 2) c_angleY = (PI / 2) - 0.001;
		else if (c_angleY < -PI / 2) c_angleY = (-PI / 2) + 0.001;
	}

	vec3 direction(
		sin(c_angleX),
		sin(c_angleY),
		cos(c_angleX)
		);
	vec3 right = vec3(
		sin(c_angleX - 3.14f / 2.f),
		0,
		cos(c_angleX - 3.14f / 2.f)
		);
	vec3 up = cross(right, direction);

	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS){
		c_position += direction * deltaTime * speed;
	}
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS){
		c_position -= direction * deltaTime * speed;
	}
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS){
		c_position += right * deltaTime * speed;
	}
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS){
		c_position -= right * deltaTime * speed;
	}


	float FoV = c_FoV;
	c_ProjectMatrix = perspective(FoV, 1500.f / 800.f, 0.1f, 100.f);
	c_ViewMatrix = lookAt(
		c_position,
		c_position + direction,
		up
		);

	lastTime = currentTime;
}


/*
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;

float* vertex;
float* normal;
float* uv;
float* indyk;
unsigned int triangles;
int main()
{
fstream plik;
string line;
int il;
plik.open("SkyBox.ply", ios::in); // ios::out | ios::app
if (plik.good() == false){
cout << "Plik nie istnieje";
exit(0);
}
getline(plik, line);
if (!(line == "ply")){
cout << "Bledny format pliku";
exit(0);
}
size_t szukana;
while (getline(plik, line)){
if ((szukana = line.find("element vertex")) != string::npos){
il = (atoi((line.substr(15, line.length() - 15)).c_str()));
vertex = new float[il * 3];
normal = new float[il * 3];
uv = new float[il * 2];
}
if ((szukana = line.find("element face")) != string::npos){
triangles = (atoi((line.substr(12, line.length() - 12)).c_str()));
indyk = new float[triangles * 4];
}
if ((szukana = line.find("end_header")) != string::npos)
break;
}

unsigned int numer = 0;
string wycinek;
for (int i = 0; i < il; i++){
numer = 0;
getline(plik, line);
while (true){

wycinek = line.substr(0, 3);
szukana = wycinek.find('-');
if (szukana != string::npos){
if (numer == 7){
wycinek = line.substr(0, 9);
line.erase(0, 9);
}
else{
wycinek = line.substr(0, 10);
line.erase(0, 10);
}
}
else{
if (numer == 7){
wycinek = line.substr(0, 8);
line.erase(0, 8);
}
else{
wycinek = line.substr(0, 9);
line.erase(0, 9);
}
}


switch (numer){
case 0:
case 1:
case 2:
vertex[(numer + (i * 3))] = atof(wycinek.c_str());
break;
case 3:
case 4:
case 5:
normal[((numer - 3) + (i * 3))] = atof(wycinek.c_str());
break;
case 6:
case 7:
uv[((numer - 6) + (i * 2))] = atof(wycinek.c_str());
break;
}
numer++;
if (numer > 7) break;
}
}
for (int i = 0; i < triangles; i++){
getline(plik, line);
line = line.substr(2, line.length());
for (int j = 0; j < 4; j++){
szukana = line.find(" ");
if (szukana != string::npos){
indyk[j + (i * 4)] = atoi((line.substr(0, szukana + 1)).c_str());
line.erase(0, szukana + 1);
}
else{
if (j + (i * 4) < 10)
indyk[j + (i * 4)] = atoi((line.substr(0, 1)).c_str());
else
indyk[j + (i * 4)] = atoi((line.substr(0, 2)).c_str());
}
}
}
cout << "~Czytanie Pliku~" << endl << endl
<< "Vertex : " << endl << endl;
for (int i = 0; i < il * 3; i++) cout << vertex[i] << endl
<< endl;
cout << "Normal :" << endl << endl;
for (int i = 0; i < il * 3; i++) cout << normal[i] << endl;
cout << endl
<< "UV :" << endl << endl;
for (int i = 0; i < il * 2; i++) printf("%f\n", uv[i]);
cout << endl <<
"Indyk : " << endl << endl;
for (int i = 0; i < triangles * 4; i++) cout << indyk[i] << endl;

plik.close();
delete[] vertex;
delete[] normal;
delete[] uv;
delete[] indyk;

getchar();
return 0;
}*/