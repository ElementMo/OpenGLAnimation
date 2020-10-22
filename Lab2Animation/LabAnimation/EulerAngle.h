#pragma once
#include <GL/freeglut.h>
#include <iostream>

class EulerAngle {
public:
	float x;
	float y;
	float z;
	EulerAngle();
	EulerAngle(float x, float y, float z);
	GLfloat* toRotMatrix();
	EulerAngle mult(float scale);
};