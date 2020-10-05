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
	float dot(const Quaternion& q) const;
	float mod();
	void normalize();
	Quaternion H(const Quaternion& q) const;
	Quaternion conj() const;
	float modSquare() const;
	Quaternion rotateBy(const Quaternion& q);
	GLfloat* toRotMatrix();
};