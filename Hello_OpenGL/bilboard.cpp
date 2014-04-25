#include "bilboard.h"


bilboard::bilboard(GLfloat poz_x, GLfloat poz_y, GLfloat poz_z, GLfloat b_size)
{
	GLfloat square[] = {
		-0.1, 0, 0,
		0.1, 0, 0,
		-0.1, 0.2, 0,

		0.1, 0.2, 0,
	};
	GLfloat color[4 * 3];
	for (unsigned i = 0; i < 4 * 3; i++) color[i] = (rand() % 1000 + 1) / 1000.f;
	GLuint indor[] = {
		0, 1, 2,
		2, 1, 3
	};

	b_scale(b_size, b_size, b_size);
	b_tranform(poz_x, poz_y, poz_z);


	glGenBuffers(1, &indorID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indorID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 4, indor, GL_STATIC_DRAW);


	glGenBuffers(1, &Vertex);
	glBindBuffer(GL_ARRAY_BUFFER, Vertex);
	glBufferData(GL_ARRAY_BUFFER, 12 * 4, square, GL_STATIC_DRAW);

	glGenBuffers(1, &Color);
	glBindBuffer(GL_ARRAY_BUFFER, Color);
	glBufferData(GL_ARRAY_BUFFER, (4 * 3) * 4, color, GL_STATIC_DRAW);
}
GLuint bilboard::getIndyk(){
	return indorID;
}
GLuint bilboard::useVertex(){
	return Vertex;
}
GLuint bilboard::useColor(){
	return Color;
}
GLfloat bilboard::getPozX(){
	return b_pozX;
}
GLfloat bilboard::getPozY(){
	return b_pozY;
}
GLfloat bilboard::getPozZ(){
	return b_pozZ;
}
void bilboard::b_scale(GLfloat s_pozX, GLfloat s_pozY, GLfloat s_pozZ){
	scaleModel = scale(vec3(s_pozX, s_pozY, s_pozZ));
	b_pozX += s_pozX;
	b_pozY += s_pozY;
}
void bilboard::b_tranform(GLfloat t_pozX, GLfloat t_pozY, GLfloat t_pozZ){
	transform = translate(vec3(t_pozX, t_pozY, t_pozZ));
	b_pozX = t_pozX;
	b_pozY = t_pozY;
	b_pozZ = t_pozZ;
}
mat4 bilboard::getModelMatrix(){
	modelMatrix = scaleModel * transform;
	return modelMatrix;
}

bilboard::~bilboard()
{
}
