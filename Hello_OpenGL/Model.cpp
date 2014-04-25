#include "Model.h"


int GetStrLen(const char* str){
	for (int i = 0;; i++){ if (str[i] == 0)return i; }
}
char* FindStr(char* array, const char* str){
	while (*array != 0){
		for (int i = 0; array[i] == str[i]; i++){
			if (str[i + 1] == 0) return array;
		}
		array++;
	}
	return NULL;
}
char* GetStrSeg(const char* array, int elements_to_copy){
	char* segment = new char[elements_to_copy];

	for (int i = 0; i < elements_to_copy && array[i] != 0; i++){
		segment[i] = array[i];
		if (i == elements_to_copy - 1) return segment;
	}
	return NULL;
}




Model::Model(const char* link,
	GLfloat poz_x, GLfloat poz_y, GLfloat poz_z,
	GLfloat size_x, GLfloat size_y, GLfloat size_z)
{
	Load(link);

	Run(poz_x, poz_y, poz_z);
	Size(size_x, size_y, size_z);
}

bool Model::Load(const char* filename){
	FILE* file;
	if (fopen_s(&file, filename, "r") != 0){
		perror("Wystapil blad");
		return false;
	}
	fseek(file, 0, SEEK_END);
	int pos = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* text = new char[pos];
	char* text_p = text;
	fread(text, 1, pos, file);
	fclose(file);


	//Sprawdzanie Formatu
	if (text[0] != 'p'
		||
		text[1] != 'l'
		||
		text[2] != 'y'){
		printf("Bledny format pliku\n");
		delete[] text;
		return false;
	}
	//Tworzenie tablic
	float* vertex;
	float* normal;
	float* uv;
	std::vector<int> indyk;

	int face;
	int array_elements;
	//
	text = FindStr(text, "element vertex") + GetStrLen("element vertex ");
	array_elements = atoi(text);
	vertex = new float[array_elements * 3];
	normal = new float[array_elements * 3];
	uv = new float[array_elements * 2];

	text = FindStr(text, "element face") + GetStrLen("element face ");
	face = atoi(text);
	indyk.reserve(face * 3);
	//Definiowanie tablic
	text = FindStr(text, "end_header") + GetStrLen("end_header\n");
	float value;
	for (int i = 0; i < array_elements; i++){
		for (int j = 0; j < 8; j++){
			value = atof(text);
			switch (j){
			case 0:
			case 1:
			case 2:
				vertex[(i * 3) + j] = value;
				break;
			case 3:
			case 4:
			case 5:
				normal[(i * 3) + (j - 3)] = value;
				break;
			case 6:
			case 7:
				uv[(i * 2) + (j - 6)] = value;
				break;
			};
			if (*text == '-')text += 10;
			else if (*text != '-')text += 9;
		}
	}
	//Indyk
	int v[4];
	for (int i = 0; i < face; i++){
		value = atof(text);
		text += 2;
		if ((int)value == 3){
			for (int j = 0; j < 3; j++){
				indyk.push_back(atoi(text));
				if (j != 2) text = FindStr(text, " ") + 1;
				else text = FindStr(text, "\n") + 1;
			}
		}
		else if ((int)value == 4){
			for (int j = 0; j < 4; j++){
				v[j] = atoi(text);
				if (j != 3) text = FindStr(text, " ") + 1;
				else text = FindStr(text, "\n") + 1;
			}
			indyk.push_back(v[0]);
			indyk.push_back(v[1]);
			indyk.push_back(v[2]);
			indyk.push_back(v[0]);
			indyk.push_back(v[2]);
			indyk.push_back(v[3]);
		}
	}
	triangles_number = indyk.size();

	glGenBuffers(4, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (array_elements * 3) * 4, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (array_elements * 3) * 4, normal, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (array_elements * 2) * 4, uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indyk.size() * 4, &indyk[0], GL_STATIC_DRAW);
	//~~~~~
	delete[] text_p;

	delete[] vertex;
	delete[] normal;
	delete[] uv;
	indyk.clear();

	return true;
}

void Model::Run(GLfloat poz_x, GLfloat poz_y, GLfloat poz_z){
	x = poz_x;
	y = poz_y;
	z = poz_z;

	Matrix = scale(vec3(s_x, s_y, s_z)) * translate(vec3(x, y, z));
}
void Model::Size(GLfloat size_x, GLfloat size_y, GLfloat size_z){
	s_x = size_x;
	s_y = size_y;
	s_z = size_z;

	Matrix = scale(vec3(s_x, s_y, s_z)) * translate(vec3(x, y, z));
}
void Model::Draw(mat4 mvp){
	mat4 newMatrix = mvp * Matrix;
	glUniformMatrix4fv(0, 1, GL_FALSE, &newMatrix[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glDrawElements(GL_TRIANGLES, triangles_number, GL_UNSIGNED_INT, 0);//liczbe wierzochlkow sie podaje
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

Model::~Model()
{
	glDeleteBuffers(4, vbo);
}