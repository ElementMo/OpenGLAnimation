#pragma once
#include "Model.h"
#include "Utils.h"

class RigidBody {
public:
	Model model;
	Model convex_hull;
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;

	glm::vec3 force;
	glm::vec3 velocity;
	glm::vec3 position;
	float mass;

	glm::vec3 torque;
	glm::quat angularVelocity;
	glm::quat rotation;
	glm::mat3 inertiaTensorMat;
	glm::mat3 inverseInertiaTensorMat;


	bool isStatic;

	RigidBody(Model& model, glm::vec3 pos, float density);
	RigidBody(Model& model, Model& convex_hull, glm::vec3 pos, float density);
	void setStatic(bool isStatic);

private:
	void ComputeMass(float density);
};