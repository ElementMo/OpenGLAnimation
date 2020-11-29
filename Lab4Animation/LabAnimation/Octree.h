#pragma once
#include "OTBox.h"
#include "OTPoint.h"
#include <vector>
#include "Utils.h"


class Octree {
private:
	OTBox boundary;
	std::vector<OTPoint> points;
	int capacity;
	bool isDivided;
	// I came up with some stupid naming... e.g. ulf meaning: Up-Left-Front
	Octree* ulf;
	Octree* urf;
	Octree* ulb;
	Octree* urb;
	Octree* dlf;
	Octree* drf;
	Octree* dlb;
	Octree* drb;

public:
	Octree();
	~Octree();
	Octree(OTBox& boundary, int n);
	void SubDivide();
	bool Insert(OTPoint& p);
	std::vector<OTPoint> Query(OTBox& queryBoundary, std::vector<OTPoint> found);
	void Show(const glm::mat4 m_vp);
	void Clear();
};