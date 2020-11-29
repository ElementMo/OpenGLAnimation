#pragma once
#include "OTPoint.h"

class OTBox {
public:
	float x, y, z;
	float w, h, d;
	OTBox();
	OTBox(float x, float y, float z, float w, float h, float d);
	OTBox(OTBox& other);
	bool Contains(OTPoint& p);
	bool Intersects(OTBox& range);
};
