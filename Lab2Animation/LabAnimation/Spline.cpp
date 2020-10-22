#include "Spline.h"

Spline::Spline()
{
}

Spline::Spline(CURVE_TYPE _curveType)
{
	curveType = _curveType;
}

void Spline::render(const GLfloat* m_mvp, int ctrlPointIndex)
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glLoadMatrixf(m_mvp);
	glColor3f(0.5f, 0.5f, 0.5f);
	// Draw Contrl Lines
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < int(ctrlPoints_pos.size()) - 1; i++) {
		glVertex3f(ctrlPoints_pos[i].x, ctrlPoints_pos[i].y, ctrlPoints_pos[i].z);
		glVertex3f(ctrlPoints_pos[i + 1].x, ctrlPoints_pos[i + 1].y, ctrlPoints_pos[i + 1].z);
	}
	glEnd();

	// Draw Control Points
	for (int i = 0; i < ctrlPoints_pos.size(); i++) {
		if (i == ctrlPointIndex) {
			glColor3f(1, 0.6f, 0.5f);
		}
		else {
			glColor3f(1, 1, 1);
		}
		//glDrawCircle(ctrlPoints_pos[i].x, ctrlPoints_pos[i].y, ctrlPoints_pos[i].z, 0.1f);
		glPushMatrix();
		glTranslatef(ctrlPoints_pos[i].x, ctrlPoints_pos[i].y, ctrlPoints_pos[i].z);
		glutSolidSphere(0.08f, 36, 18);
		glPopMatrix();

		glDrawAxis(ctrlPoints_pos[i].x, ctrlPoints_pos[i].y, ctrlPoints_pos[i].z,
			ctrlPoints_euler[i].x, ctrlPoints_euler[i].y, ctrlPoints_euler[i].z,
			0.5f
		);
	}

	renderPoints_pos = SampleCurve();

	// Darw Curve
	glLineWidth(1.5f);
	glBegin(GL_LINES);
	glColor3f(0.9f, 0.9f, 0.9f);
	for (int i = 1; i < int(renderPoints_pos.size()) - 1; i++) {
		glVertex3f(renderPoints_pos[i].x, renderPoints_pos[i].y, renderPoints_pos[i].z);
		glVertex3f(renderPoints_pos[i + 1].x, renderPoints_pos[i + 1].y, renderPoints_pos[i + 1].z);
	}
	glEnd();

	glPopMatrix();
}

std::vector<Vec3> Spline::SampleCurve() {
	std::vector<Vec3> samplePoints;
	for (int i = 0; i < int(ctrlPoints_pos.size()) - 3; i++) {
		for (int j = 0; j <= renderFragment; j++) {
			samplePoints.push_back(posAtT(float(j) / renderFragment, i));
		}
	}
	return samplePoints;
}


//void Spline::addPoints(const Vec3& pos, const Quaternion& quat)
//{
//	ctrlPoints_pos.push_back(pos);
//	ctrlPoints_quat.push_back(quat);
//
//	total_t = int(ctrlPoints_pos.size()) - 3;
//}

void Spline::addPoints(const Vec3& pos, const EulerAngle& euler)
{
	ctrlPoints_pos.push_back(pos);
	ctrlPoints_euler.push_back(euler);
	addQuats(fromEuler(euler.x, euler.y, euler.z));
}

void Spline::addQuats(const Quaternion& quat)
{
	ctrlPoints_quat.push_back(quat);
	total_t = int(ctrlPoints_pos.size()) - 3;
}

Vec3 Spline::posAtT(float t, int patch)
{
	Vec3 result;
	float TM[4];
	float T[4] = { t * t * t, t * t, t, 1 };
	if (curveType == CATMULL_ROM) {
		TM[0] = TxM(T, blendMat_CatmullRom)[0];
		TM[1] = TxM(T, blendMat_CatmullRom)[1];
		TM[2] = TxM(T, blendMat_CatmullRom)[2];
		TM[3] = TxM(T, blendMat_CatmullRom)[3];
	}
	else if (curveType == B_SPLINE) {
		TM[0] = TxM(T, blendMat_BSpline)[0];
		TM[1] = TxM(T, blendMat_BSpline)[1];
		TM[2] = TxM(T, blendMat_BSpline)[2];
		TM[3] = TxM(T, blendMat_BSpline)[3];
	}

	float patch_x[4] = { ctrlPoints_pos[patch].x, ctrlPoints_pos[patch + 1].x, ctrlPoints_pos[patch + 2].x, ctrlPoints_pos[patch + 3].x };
	float patch_y[4] = { ctrlPoints_pos[patch].y, ctrlPoints_pos[patch + 1].y, ctrlPoints_pos[patch + 2].y, ctrlPoints_pos[patch + 3].y };
	float patch_z[4] = { ctrlPoints_pos[patch].z, ctrlPoints_pos[patch + 1].z, ctrlPoints_pos[patch + 2].z, ctrlPoints_pos[patch + 3].z };
	result.x = TMxG(TM, patch_x);
	result.y = TMxG(TM, patch_y);
	result.z = TMxG(TM, patch_z);

	return result;
}

EulerAngle Spline::eulerAtT(float t, int patch)
{
	EulerAngle e;
	EulerAngle e_1 = ctrlPoints_euler[patch + 1];
	EulerAngle e_2 = ctrlPoints_euler[patch + 2];
	e = lerp(e_1, e_2, t);
	return e;
}

Quaternion Spline::quatAtT(float t, int patch)
{
	Quaternion q;
	Quaternion q_1 = ctrlPoints_quat[patch + 1];
	Quaternion q_2 = ctrlPoints_quat[patch + 2];
	q = slerp(q_1, q_2, t);
	return q;
}
