#pragma once

class OTPoint {
public:
	float x, y, z;
	int index;
	OTPoint(float x, float y, float z, int i) : x(x), y(y), z(z), index(i) {}
};