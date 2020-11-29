#include "Boid.h"

Boid::Boid(Model& model, float x, float y, float z) {
	this->model = model;
	acceleration = glm::vec3(0);
	velocity = glm::vec3(Random(-PI, PI), Random(-PI, PI), Random(-PI, PI));
	position = glm::vec3(x, y, z);
	radius = 3.0f;
	maxSpeed = 0.5f;
	maxForce = 0.05f;
}

void Boid::Simulate(std::vector<Boid> boids) {
	Flock(boids);
	Update();
}

void Boid::ApplyForce(glm::vec3 force) {
	acceleration += force;
}

void Boid::Flock(std::vector<Boid> boids) {
	glm::vec3 seperate = Seperate(boids);
	glm::vec3 align = Align(boids);
	glm::vec3 cohesion = Cohesion(boids);

	ApplyForce(seperate);
	ApplyForce(align);
	ApplyForce(cohesion);
}

void Boid::Update() {
	velocity += acceleration;
	velocity = glm::normalize(velocity) * maxSpeed;
	position += velocity;
	acceleration *= 0;
}

glm::vec3 Boid::SteerTo(glm::vec3 target)
{
	glm::vec3 desired = target - position;
	desired = glm::normalize(desired) * maxSpeed;
	glm::vec3 steer = desired - velocity;
	steer = glm::normalize(steer) * maxForce;
	return steer;
}

glm::vec3 Boid::Seperate(std::vector<Boid> boids)
{
	glm::vec3 steer_sum = glm::vec3(0);
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		float d = glm::distance(position, boids[i].position);
		if (d > 0 && d < radius / 2) {
			glm::vec3 relative_v = position - boids[i].position;
			steer_sum += relative_v;
			count++;
		}
	}
	if (count > 0) {
		steer_sum /= (float)count;
	}
	if (glm::length(steer_sum) > 0.01f) {
		steer_sum = glm::normalize(steer_sum) * maxSpeed;
		steer_sum = steer_sum - velocity;
		steer_sum = glm::normalize(steer_sum) * maxForce;
	}
	return steer_sum;
}

glm::vec3 Boid::Align(std::vector<Boid> boids)
{
	glm::vec3 steer_sum = glm::vec3(0);
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		float d = glm::distance(position, boids[i].position);
		if (d > 0 && d < radius) {
			steer_sum += boids[i].velocity;
			count++;
		}
	}
	if (count > 0) {
		steer_sum /= (float)count;
		steer_sum = glm::normalize(steer_sum) * maxSpeed;
		steer_sum = steer_sum - velocity;
		steer_sum = glm::normalize(steer_sum) * maxForce;
	}
	return steer_sum;
}

glm::vec3 Boid::Cohesion(std::vector<Boid> boids)
{
	glm::vec3 pos_sum = glm::vec3(0);
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		float d = glm::distance(position, boids[i].position);
		if (d > 0 && d < radius) {
			pos_sum += boids[i].position;
			count++;
		}
	}
	if (count > 0) {
		pos_sum /= (float)count;
		return SteerTo(pos_sum);
	}
	return glm::vec3(0);
}
