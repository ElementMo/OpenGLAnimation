#pragma once
#include "Model.h"
#include "Utils.h"

class Boid {
public:
	Model model;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float radius;
	float maxForce;
	float maxSpeed;

	Boid(Model& model, float x, float y, float z);
	void Simulate(std::vector<Boid> boids);

	void ApplyForce(glm::vec3 force);
	void Flock(std::vector<Boid> boids);
	void Update();
	glm::vec3 SteerTo(glm::vec3 target);
	glm::vec3 Seperate(std::vector<Boid> boids);
	glm::vec3 Align(std::vector<Boid> boids);
	glm::vec3 Cohesion(std::vector<Boid> boids);
};