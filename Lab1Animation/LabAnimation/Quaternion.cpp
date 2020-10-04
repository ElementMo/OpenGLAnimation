#include "Quaternion.h"

Quaternion::Quaternion() : w(1), x(0), y(0), z(0)
{
}

Quaternion::Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z)
{
}


Quaternion::~Quaternion()
{
}


float Quaternion::dot(const Quaternion& q)
{
	return w * q.w + x * q.x + y * q.y + z * q.z;
}
Quaternion Quaternion::H(const Quaternion& q)
{
	// Hamilton product
	// https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
	float coefficient_w = w * q.w - x * q.x - y * q.y - z * q.z;
	float coefficient_x = w * q.x + x * q.w + y * q.z - z * q.y;
	float coefficient_y = w * q.y - x * q.z + y * q.w + z * q.x;
	float coefficient_z = w * q.z + x * q.y - y * q.x + z * q.w;
	return Quaternion(coefficient_w, coefficient_x, coefficient_y, coefficient_z);
}

Quaternion Quaternion::conj()
{
	float modSquare = this->modSquare();
	return Quaternion(w / modSquare, -x / modSquare, -y / modSquare, -z / modSquare);
}

float Quaternion::modSquare()
{
	return w * w + x * x + y * y + z * z;
}

Quaternion Quaternion::rotateBy(const Quaternion& q)
{

	return Quaternion();
}

