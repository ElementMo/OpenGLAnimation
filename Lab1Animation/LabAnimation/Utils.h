#pragma once
#include "Quaternion.h"

float map(float val, float a, float b, float c, float d);
float lerp(float start, float stop, float amt);
Quaternion slerp(Quaternion* q1, Quaternion* q2, float t);