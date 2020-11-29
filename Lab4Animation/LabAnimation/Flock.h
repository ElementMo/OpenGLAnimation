#pragma once
#include "Boid.h"
#include "Utils.h"

class Flock {
public:
	std::vector<Boid> boids;


	float light_position[4];
	float light_ambient[4];
	float light_diffuse[4];
	float light_specular[4];

	Flock();
	void Simulate();
	void Render(const glm::mat4 m_vp);
	void AddBoid(Boid& boid);
	void SetLighting(float pos[4], float amb[4], float diff[4], float spec[4]);
	void Reset();

	void SetRadius(float r);
	void SetMaxSpeed(float s);
	void SetMaxForce(float f);
};