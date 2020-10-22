#include "Utils.h"

float map(float val, float a, float b, float c, float d) {
	float result = c + (d - c) * ((val - a) / (b - a));
	return result;
}

float constrain(float amt, float low, float high) {
	return (amt < low) ? low : ((amt > high) ? high : amt);
}

float lerp(float start, float stop, float amt) {
	return start + (stop - start) * amt;
}

// Lerp vs Slerp
// http://www.codewee.com/view.php?idx=42
// https://tiborstanko.sk/lerp-vs-slerp.html

Quaternion slerp(const Quaternion& q1, const Quaternion& q2, const float t)
{
	Quaternion P;
	float omega = acos(q1.dot(q2));
	float sinOmega = sin(omega);
	if (sinOmega == 0) {
		return lerp(q1, q2, t);
	}
	else {
		float t_1 = 1 - t;
		float coefficient_q1 = sin(t_1 * omega) / sinOmega;
		float coefficient_q2 = sin(t * omega) / sinOmega;
		P.w = coefficient_q1 * q1.w + coefficient_q2 * q2.w;
		P.x = coefficient_q1 * q1.x + coefficient_q2 * q2.x;
		P.y = coefficient_q1 * q1.y + coefficient_q2 * q2.y;
		P.z = coefficient_q1 * q1.z + coefficient_q2 * q2.z;
		P.normalize();
		return P;
	}
}

Quaternion lerp(const Quaternion& q1, const Quaternion& q2, const float t) {
	Quaternion P;
	float t_1 = 1 - t;
	P.w = t_1 * q1.w + t * q2.w;
	P.x = t_1 * q1.x + t * q2.x;
	P.y = t_1 * q1.y + t * q2.y;
	P.z = t_1 * q1.z + t * q2.z;
	P.normalize();
	return P;
}

EulerAngle lerp(const EulerAngle& e1, const EulerAngle& e2, const float t)
{
	float t_1 = 1 - t;
	EulerAngle E;
	E.x = t_1 * e1.x + t * e2.x;
	E.y = t_1 * e1.y + t * e2.y;
	E.z = t_1 * e1.z + t * e2.z;
	return E;
}

Quaternion fromEuler(float x, float y, float z)
{
	float cosZ = cos(z / 2);
	float sinZ = sin(z / 2);
	float cosY = cos(y / 2);
	float sinY = sin(y / 2);
	float cosX = cos(x / 2);
	float sinX = sin(x / 2);

	Quaternion q;
	q.w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	q.x = sinX * cosY * cosZ - cosX * sinY * sinZ;
	q.y = cosX * sinY * cosZ + sinX * cosY * sinZ;
	q.z = cosX * cosY * sinZ - sinX * sinY * cosZ;
	//q.normalize();
	return q;

}

float* TxM(const float T[4], const float M[16])
{
	float result[4] = { 0 };
	result[0] = T[0] * M[0] + T[1] * M[1] + T[2] * M[2] + T[3] * M[3];
	result[1] = T[0] * M[4] + T[1] * M[5] + T[2] * M[6] + T[3] * M[7];
	result[2] = T[0] * M[8] + T[1] * M[9] + T[2] * M[10] + T[3] * M[11];
	result[3] = T[0] * M[12] + T[1] * M[13] + T[2] * M[14] + T[3] * M[15];
	return result;
}

float TMxG(const float TM[4], const float G[4])
{
	float result = 0;
	result = TM[0] * G[0] + TM[1] * G[1] + TM[2] * G[2] + TM[3] * G[3];
	return result;
}

void glDrawCircle(float x, float y, float z, float size) {
	glBegin(GL_POLYGON);
	for (float angle = 0.0f; angle <= 6.28f; angle += 0.5f)
	{
		float cx = x + size / 2 * sin(angle);
		float cy = y + size / 2 * cos(angle);
		glVertex3f(cx, cy, z);
	}
	glEnd();
}

void glDrawAxis(float x, float y, float z, float rx, float ry, float rz, float size)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	// Order matters, must follow Z - Y - X order
	glRotatef(rz * RAD_TO_DEG, 0.0, 0.0, 1.0);
	glRotatef(ry * RAD_TO_DEG, 0.0, 1.0, 0.0);
	glRotatef(rx * RAD_TO_DEG, 1.0, 0.0, 0.0);
	// draw our axes
	glLineWidth(2);
	glBegin(GL_LINES);
	// x axis
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(size, 0.0, 0.0);
	// y axis
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, size, 0.0);
	// z axis
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, size);
	glEnd();
	glPopMatrix();
}
