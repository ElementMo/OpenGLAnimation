#include "Flock.h"

Flock::Flock() {
}

void Flock::Simulate()
{
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].Simulate(boids);
	}
}

void Flock::Render(const glm::mat4 m_vp)
{
	for (int i = 0; i < boids.size(); i++)
	{
		glm::mat4 m_translate = glm::translate(glm::mat4(1.0f), boids[i].position);
		glm::mat4 m_rotation = glm::inverse(glm::lookAt(glm::vec3(0), glm::normalize(boids[i].velocity),  glm::vec3(0, 1, 0)));
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
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].acceleration *= 0;
		boids[i].velocity = glm::vec3(Random(-PI, PI), Random(-PI, PI), Random(-PI, PI));
		boids[i].position = glm::vec3(Random(-PI, PI), Random(-PI, PI), Random(-PI, PI));
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