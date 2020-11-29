#include "Flock.h"

Flock::Flock(float flockSize) {
	this->flockSize = flockSize;
}

void Flock::Simulate()
{
	// Re-create octree
	delete octree;
	OTBox boundary(0, 0, 0, flockSize, flockSize / 2, flockSize / 2);
	octree = new Octree(boundary, 5);

	// Insert boid into octree
	for (int i = 0; i < boids.size(); i++) {
		OTPoint p(boids[i].position.x, boids[i].position.y, boids[i].position.z, i);
		octree->Insert(p);
	}
	// retrieve nearby boids for each boid
	for (int i = 0; i < boids.size(); i++)
	{
		OTBox range(boids[i].position.x, boids[i].position.y, boids[i].position.y, boids[i].radius, boids[i].radius, boids[i].radius);
		std::vector<OTPoint> nearbyPoints;
		nearbyPoints = octree->Query(range, nearbyPoints);
		std::vector<Boid> nearby;
		for (int i = 0; i < nearbyPoints.size(); i++) {
			nearby.push_back(boids[nearbyPoints[i].index]);
		}
		boids[i].Simulate(nearby);
		boids[i].BoundaryCheck(flockSize, flockSize / 2, flockSize / 2);
	}

}

void Flock::Render(const glm::mat4 m_vp, bool showOctree)
{
	if (showOctree)
		octree->Show(m_vp);

	for (int i = 0; i < boids.size(); i++)
	{
		glm::mat4 m_translate = glm::translate(glm::mat4(1.0f), boids[i].position);
		glm::mat4 m_rotation = glm::inverse(glm::lookAt(glm::vec3(0), glm::normalize(boids[i].velocity), glm::vec3(0, 1, 0)));
		glm::mat4 m_boid = m_translate * m_rotation;
		boids[i].model.setLighting(light_position, light_ambient, light_diffuse, light_specular);
		boids[i].model.render(m_vp, m_boid, true);
	}
}

void Flock::AddBoid(Boid& boid)
{
	boids.push_back(boid);
}

void Flock::SetLighting(float pos[4], float amb[4], float diff[4], float spec[4])
{
	for (int i = 0; i < 4; i++) {
		light_position[i] = pos[i];
		light_ambient[i] = amb[i];
		light_diffuse[i] = diff[i];
		light_specular[i] = spec[i];
	}
}

void Flock::Reset() {
	int range = flockSize/2;
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].acceleration *= 0;
		boids[i].velocity = glm::vec3(Random(-range, range), Random(-range, range), Random(-range, range));
		boids[i].position = glm::vec3(Random(-range, range), Random(-range, range), Random(-range, range));
	}
}

void Flock::SetRadius(float r) {
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].radius = r;
	}
}

void Flock::SetMaxSpeed(float s) {
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].maxSpeed = s;
	}
}

void Flock::SetMaxForce(float f) {
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].maxForce = f;
	}
}