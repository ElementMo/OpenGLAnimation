#include "RigidBody.h"

RigidBody::RigidBody(Model& model, glm::vec3 pos, float density)
{
	this->model = model;
	this->convex_hull = model;
	this->force = glm::vec3(0);
	this->velocity = glm::vec3(0);
	this->position = pos;
	this->mass = 0;

	this->torque = glm::vec3(0);
	this->angularVelocity = glm::vec3(0);
	this->rotation = glm::quat();
	this->inertiaTensorMat = glm::mat3(0);
	this->isStatic = false;

	this->verts = convex_hull.verts;
	this->normals = convex_hull.normals;

	ComputeMass(density);
}

RigidBody::RigidBody(Model& model, Model& convex_hull, glm::vec3 pos, float density)
{
	this->model = model;
	this->convex_hull = convex_hull;
	this->force = glm::vec3(0);
	this->velocity = glm::vec3(0);
	this->position = pos;
	this->mass = 0;

	this->torque = glm::vec3(0);
	this->angularVelocity = glm::vec3(0);
	this->rotation = glm::quat();
	this->inertiaTensorMat = glm::mat3(0);
	this->isStatic = false;

	this->verts = convex_hull.verts;
	this->normals = convex_hull.normals;

	ComputeMass(density);
}

void RigidBody::setStatic(bool isStatic) {
	this->isStatic = isStatic;
}

void RigidBody::ComputeMass(float density) {
	glm::vec3 max_dist = glm::vec3(0);
	for (size_t i = 0; i < this->verts.size(); i++) {
		for (size_t j = 0; j < 3; j++) {
			float vert_dist = this->verts[i][j];
			max_dist[i] = vert_dist > max_dist[i] ? vert_dist : max_dist[i];
		}
	}
	this->mass = max_dist.x * max_dist.y * max_dist.z * density;
	this->inertiaTensorMat[0][0] = this->mass * max_dist.x * max_dist.x;
	this->inertiaTensorMat[1][1] = this->mass * max_dist.y * max_dist.y;
	this->inertiaTensorMat[2][2] = this->mass * max_dist.z * max_dist.z;

	this->inverseInertiaTensorMat = glm::inverse(this->inertiaTensorMat);

	//std::cout << mass << glm::to_string(inertiaTensorMat) << std::endl;
}