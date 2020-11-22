#include "EulerAngle.h"

EulerAngle::EulerAngle() : x(0), y(0), z(0){}

EulerAngle::EulerAngle(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

GLfloat* EulerAngle::toRotMatrix()
{
	float cos_alpha = cos(x);
	float sin_alpha = sin(x);
	float cos_beta = cos(y);
	float sin_beta = sin(y);
	float cos_gama = cos(z);
	float sin_gama = sin(z);

	GLfloat rotMatrix[16];
	rotMatrix[0] = cos_gama * cos_beta;
	rotMatrix[1] = sin_gama * cos_beta;
	rotMatrix[2] = -sin_beta;
	rotMatrix[3] = 0.0f;
	rotMatrix[4] = -sin_gama * cos_alpha + cos_gama * sin_beta * sin_alpha;
	rotMatrix[5] = cos_gama * cos_alpha + sin_gama * sin_beta * sin_alpha;
	rotMatrix[6] = cos_beta * sin_alpha;
	rotMatrix[7] = 0.0f;
	rotMatrix[8] = sin_gama * sin_alpha + cos_gama * sin_beta * cos_alpha;
	rotMatrix[9] = -cos_gama * sin_alpha + sin_gama * sin_beta * cos_alpha;
	rotMatrix[10] = cos_beta * cos_alpha;
	rotMatrix[11] = 0.0f;
	rotMatrix[12] = 0.0f;
	rotMatrix[13] = 0.0f;
	rotMatrix[14] = 0.0f;
	rotMatrix[15] = 1.0f;
	return rotMatrix;
}

EulerAngle EulerAngle::mult(float scale) {
	EulerAngle e;
	e.x = this->x * scale;
	e.y = this->y * scale;
	e.z = this->z * scale;
	return e;
}