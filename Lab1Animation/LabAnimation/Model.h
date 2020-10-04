#pragma once
#include <GL/freeglut.h>

class Model
{
private:
	float m_model_pos[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	float m_model_rot[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1,
		0, 0, 0, 1
	};
	const char* fileName;
	float model_color[3] = { 1, 1, 1 };

public:
	Model();
	Model(float color[3]);
	Model(const char* fileName, float color[3]);
	~Model();
	void translate(float x, float y, float z);
	void rotate(float x, float y, float z);
	void rotate(float w, float x, float y, float z);
	void scale(float x, float y, float z);
	void scale(float s);
	void render(const GLfloat* m_mvp);
};