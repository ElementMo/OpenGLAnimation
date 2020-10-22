#pragma once
#include "Quaternion.h"
#include "EulerAngle.h"
#include <cassert>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#define PI 3.1415927f
#define TAU 6.2831855f
#define EPSILON 1.0E-4f
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.295776f


float map(float val, float a, float b, float c, float d);
float constrain(float amt, float low, float high);
float lerp(float start, float stop, float amt);
Quaternion slerp(const Quaternion& q1, const Quaternion& q2, const float t);
Quaternion lerp(const Quaternion& q1, const Quaternion& q2, const float t);
EulerAngle lerp(const EulerAngle& e1, const EulerAngle& e2, const float t);
Quaternion fromEuler(float x, float y, float z);
float* TxM(const float T[4], const float M[16]);
float TMxG(const float TM[4], const float G[4]);
void glDrawCircle(float x, float y, float z, float size);
void glDrawAxis(float x, float y, float z, float rx, float ry, float rz, float size);

struct Vec3 {
	float x, y, z;
	Vec3() : x(0), y(0), z(0){}
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
	float operator [](const int i) { assert(i < 3); return i > 0 ? (i > 1 ? z : y) : x; };
};
enum CURVE_TYPE {
	CATMULL_ROM,
	B_SPLINE
};
enum ORIENTATION {
	QUATERNION,
	EULER
};