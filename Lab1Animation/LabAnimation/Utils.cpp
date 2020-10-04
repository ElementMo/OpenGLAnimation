#include "Utils.h"

float map(float val, float a, float b, float c, float d) {
	float result = c + (d - c) * ((val - a) / (b - a));
	return result;
}

float lerp(float start, float stop, float amt) {
	return start + (stop - start) * amt;
}

Quaternion slerp(Quaternion* q1, Quaternion* q2, float t)
{
	
	return Quaternion();
}
