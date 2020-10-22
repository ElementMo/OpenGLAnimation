#pragma once
#include <vector>
#include "Utils.h"

class Spline {
public:
	std::vector<Vec3> ctrlPoints_pos;
	std::vector<Quaternion> ctrlPoints_quat;
	std::vector<EulerAngle> ctrlPoints_euler;
	std::vector<Vec3> renderPoints_pos;

	float t = 0;
	float total_t = 0;
	int segmentIndex = 0;
	int renderFragment = 10;

	CURVE_TYPE curveType = B_SPLINE;

	float blendMat_CatmullRom[16] = {
		-0.5f,  1.0f, -0.5f,  0.0f,
		 1.5f, -2.5f,  0.0f,  1.0f,
		-1.5f,  2.0f,  0.5f,  0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f
	};
	float blendMat_BSpline[16] = {
		-1.0f / 6.0f,  3.0f / 6.0f, -3.0f / 6.0f,  1.0f / 6.0f,
		 3.0f / 6.0f, -6.0f / 6.0f,  0.0f,  4.0f / 6.0f,
		-3.0f / 6.0f,  3.0f / 6.0f,  3.0f / 6.0f,  1.0f / 6.0f,
		 1.0f / 6.0f, 0.0f, 0.0f, 0.0f
	};

	Spline();
	Spline(CURVE_TYPE curveType);
	void render(const GLfloat* m_mvp, int ctrlPointIndex);
	//void addPoints(const Vec3& pos, const Quaternion& quat);
	void addPoints(const Vec3& pos, const EulerAngle& euler);
	void addQuats(const Quaternion& quat);
	Vec3 posAtT(float t, int patch);
	EulerAngle eulerAtT(float t, int patch);
	Quaternion quatAtT(float t, int patch);
	std::vector<Vec3> SampleCurve();
};