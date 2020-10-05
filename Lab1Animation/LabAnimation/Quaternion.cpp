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


float Quaternion::dot(const Quaternion& q) const
{
	return w * q.w + x * q.x + y * q.y + z * q.z;
}
float Quaternion::mod()
{
	return sqrt(w * w + x * x + y * y + z * z);
}
void Quaternion::normalize()
{
	float q_mod = this->mod();
	if (q_mod != 0) {
		w /= q_mod;
		x /= q_mod;
		y /= q_mod;
		z /= q_mod;
	}
}

// MIT Quaternion Lecture
// https://web.mit.edu/2.998/www/QuaternionReport1.pdf
Quaternion Quaternion::H(const Quaternion& q) const
{
	// Hamilton product
	// https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
	float coefficient_w = w * q.w - x * q.x - y * q.y - z * q.z;
	float coefficient_x = w * q.x + x * q.w + y * q.z - z * q.y;
	float coefficient_y = w * q.y - x * q.z + y * q.w + z * q.x;
	float coefficient_z = w * q.z + x * q.y - y * q.x + z * q.w;
	return Quaternion(coefficient_w, coefficient_x, coefficient_y, coefficient_z);
}

Quaternion Quaternion::conj() const
{
	float modSquare = this->modSquare();
	return Quaternion(w / modSquare, -x / modSquare, -y / modSquare, -z / modSquare);
}

float Quaternion::modSquare() const
{
	return w * w + x * x + y * y + z * z;
}

Quaternion Quaternion::rotateBy(const Quaternion& q)
{
	return Quaternion();
}


GLfloat* Quaternion::toRotMatrix()
{
	GLfloat rotMatrix[16];
	rotMatrix[0] = 1.0f - 2 * y * y - 2 * z * z;
	rotMatrix[1] = 2 * x * y + 2 * w * z;
	rotMatrix[2] = 2 * x * z - 2 * w * y;
	rotMatrix[3] = 0.0f;
	rotMatrix[4] = 2 * x * y - 2 * w * z;
	rotMatrix[5] = 1.0f - 2 * x * x - 2 * z * z;
	rotMatrix[6] = 2 * y * z + 2 * w * x;
	rotMatrix[7] = 0.0f;
	rotMatrix[8] = 2 * x * z + 2 * w * y;
	rotMatrix[9] = 2 * y * z - 2 * w * x;
	rotMatrix[10] = 1.0f - 2 * x * x - 2 * y * y;
	rotMatrix[11] = 0.0f;
	rotMatrix[12] = 0.0f;
	rotMatrix[13] = 0.0f;
	rotMatrix[14] = 0.0f;
	rotMatrix[15] = 1.0f;
	return rotMatrix;
}

