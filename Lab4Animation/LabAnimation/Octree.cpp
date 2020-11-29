#include "Octree.h"


Octree::Octree()
{
	OTBox boundary;
	this->boundary = boundary;
	this->capacity = 4;
}

Octree::~Octree()
{
	delete ulf;
	delete urf;
	delete ulb;
	delete urb;

	delete dlf;
	delete drf;
	delete dlb;
	delete drb;
}

Octree::Octree(OTBox& boundary, int n)
{
	this->boundary = boundary;
	this->capacity = n;
}

void Octree::SubDivide()
{
	float x = this->boundary.x;
	float y = this->boundary.y;
	float z = this->boundary.z;
	float w = this->boundary.w * 0.5f;
	float h = this->boundary.h * 0.5f;
	float d = this->boundary.d * 0.5f;

	OTBox b_ulf(x - w, y + h, z + d, w, h, d);
	OTBox b_urf(x + w, y + h, z + d, w, h, d);
	OTBox b_ulb(x - w, y + h, z - d, w, h, d);
	OTBox b_urb(x + w, y + h, z - d, w, h, d);
	this->ulf = new Octree(b_ulf, this->capacity);
	this->urf = new Octree(b_urf, this->capacity);
	this->ulb = new Octree(b_ulb, this->capacity);
	this->urb = new Octree(b_urb, this->capacity);

	OTBox b_dlf(x - w, y - h, z + d, w, h, d);
	OTBox b_drf(x + w, y - h, z + d, w, h, d);
	OTBox b_dlb(x - w, y - h, z - d, w, h, d);
	OTBox b_drb(x + w, y - h, z - d, w, h, d);
	this->dlf = new Octree(b_dlf, this->capacity);
	this->drf = new Octree(b_drf, this->capacity);
	this->dlb = new Octree(b_dlb, this->capacity);
	this->drb = new Octree(b_drb, this->capacity);

	this->isDivided = true;
}

bool Octree::Insert(OTPoint& p)
{
	if (!this->boundary.Contains(p)) {
		return false;
	}
	if (points.size() < this->capacity) {
		points.push_back(p);
		return true;
	}
	else {
		if (!this->isDivided) {
			SubDivide();
		}
		if (ulf->Insert(p)) {
			return true;
		}
		else if (urf->Insert(p)) {
			return true;
		}
		else if (ulb->Insert(p)) {
			return true;
		}
		else if (urb->Insert(p)) {
			return true;
		}
		else if (dlf->Insert(p)) {
			return true;
		}
		else if (drf->Insert(p)) {
			return true;
		}
		else if (dlb->Insert(p)) {
			return true;
		}
		else if (drb->Insert(p)) {
			return true;
		}
	}
	return false;
}

std::vector<OTPoint> Octree::Query(OTBox& queryBoundary, std::vector<OTPoint> found)
{
	if (boundary.Intersects(queryBoundary)) {
		for (int i = 0; i < points.size(); i++) {
			if (queryBoundary.Contains(points[i])) {
				found.push_back(points[i]);
			}
		}
		if (isDivided) {
			found = ulf->Query(queryBoundary, found);
			found = urf->Query(queryBoundary, found);
			found = ulb->Query(queryBoundary, found);
			found = urb->Query(queryBoundary, found);

			found = dlf->Query(queryBoundary, found);
			found = drf->Query(queryBoundary, found);
			found = dlb->Query(queryBoundary, found);
			found = drb->Query(queryBoundary, found);
		}
	}
	return found;
}

void Octree::Show(const glm::mat4 m_vp)
{
	glLoadMatrixf(glm::value_ptr(m_vp));
	glDrawBox(boundary.x, boundary.y, boundary.z, boundary.w, boundary.h, boundary.d, 0.5f, 0.5f, 0.5f);

	float pSize = 0.1f;
	for (int i = 0; i < points.size(); i++)
	{
		glDrawBox(points[i].x, points[i].y, points[i].z, pSize, pSize, pSize, 0.8f, 0.8f, 0.8f);
	}

	if (isDivided) {
		ulf->Show(m_vp);
		urf->Show(m_vp);
		ulb->Show(m_vp);
		urb->Show(m_vp);

		dlf->Show(m_vp);
		drf->Show(m_vp);
		dlb->Show(m_vp);
		drb->Show(m_vp);
	}
}
