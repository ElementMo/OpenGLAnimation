#include "Model.h"

Model::Model() : fileName("teapot")
{
}

Model::Model(float color[3]) : fileName("teapot")
{
	model_color[0] = color[0];
	model_color[1] = color[1];
	model_color[2] = color[2];
}

Model::Model(const char* fileName, float color[3])
{
	this->fileName = fileName;
	model_color[0] = color[0];
	model_color[1] = color[1];
	model_color[2] = color[2];
}

Model::~Model()
{
}

void Model::translate(float x, float y, float z)
{
	m_model_pos[12] = x;
	m_model_pos[13] = y;
	m_model_pos[14] = z;
}

void Model::rotate(float x, float y, float z)
{
}

void Model::rotate(float w, float x, float y, float z)
{
}

void Model::scale(float x, float y, float z)
{
}

void Model::scale(float s)
{
}

void Model::render(const GLfloat *m_mvp)
{
	glPushMatrix();
	glLoadMatrixf(m_mvp);
	glColor3f(model_color[0], model_color[1], model_color[2]);
	if (fileName == "teapot") {
		glutWireTeapot(1);
	}
	if (fileName == "teacup") {
		glutWireTeacup(1);
	}
	if (fileName == "ground") {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		int xsize = 1, zsize = 1;
		for (int j = -10; j < 10; j += zsize)
		{
			for (int i = -10; i < 10; i += xsize)
			{
				glBegin(GL_POLYGON);
				glVertex3f(i * xsize, 0.0, j * zsize);
				glVertex3f(i * xsize, 0.0, j * zsize + zsize);
				glVertex3f(i * xsize + xsize, 0.0, j * zsize + zsize);
				glVertex3f(i * xsize + xsize, 0.0, j * zsize);
				glEnd();
			}
		}
	}
	glPopMatrix();
}
