#pragma once
#include "RigidBody.h"
#include "Model.h"
#include "Utils.h"
#include "MTV.h"

class Scene {
public:
	float total_t;
	float dt;
	int iterations;
	float attenuation = 0.7f;
	std::vector<RigidBody> rigidBodies;

	float light_position[4];
	float light_ambient[4];
	float light_diffuse[4];
	float light_specular[4];

	Scene(float dt, int iterations);
	RigidBody Add(RigidBody& body);
	void Clear();
	void Step();
	void Reset(Model& model, Model& floor);
	void Reset(Model& model, Model& convex, Model& floor, float gap, float size, float height);
	void SetLighting(float pos[4], float amb[4], float diff[4], float spec[4]);
	void Render(const glm::mat4 m_vp);
};