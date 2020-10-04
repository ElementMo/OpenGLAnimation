#pragma once
#include <GL/freeglut.h>
#include <iostream>

class Quaternion
{
private:
public:
	float w;
	float x;
	float y;
	float z;
	Quaternion();
	Quaternion(float w, float x, float y, float z);
	~Quaternion();
	float dot(const Quaternion& q);
	Quaternion H(const Quaternion& q);
	Quaternion conj();
	float modSquare();
	Quaternion rotateBy(const Quaternion& q);
	GLfloat* toRotMatrix();
};