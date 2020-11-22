#include "Scene.h"

Scene::Scene(float dt, int iterations)
{
	this->dt = dt;
	this->iterations = iterations;
}

RigidBody Scene::Add(RigidBody& body) {
	rigidBodies.push_back(body);
	return body;
}

void Scene::Clear()
{
	rigidBodies.clear();
}


// Reference: 3D Collision with Separate Axis Theorem  
// https://gamedev.stackexchange.com/questions/44500/how-many-and-which-axes-to-use-for-3d-obb-collision-with-sat#comment162883_92055
// Reference: Collision Detection Using the Separating Axis Theorem
// https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
// Reference: SAT (Separating Axis Theorem)
// http://www.dyn4j.org/2010/01/sat/#sat-top

bool SeperateOnAxis(std::vector<glm::vec3> vertsA, std::vector<glm::vec3> vertsB, glm::vec3 axis) {
	//if (axis == glm::vec3(0)) {
	//	return false;
	//}

	float aMin = std::numeric_limits<float>::max();
	float aMax = std::numeric_limits<float>::min();
	float bMin = std::numeric_limits<float>::max();
	float bMax = std::numeric_limits<float>::min();

	// Calculate interval a's min/max
	for (int i = 0; i < vertsA.size(); i++) {
		float aDist = glm::dot(vertsA[i], axis);
		aMin = aDist < aMin ? aDist : aMin;
		aMax = aDist > aMax ? aDist : aMax;
	}
	// Calculate interval b's min/max
	for (int i = 0; i < vertsB.size(); i++) {
		float bDist = glm::dot(vertsB[i], axis);
		bMin = bDist < bMin ? bDist : bMin;
		bMax = bDist > bMax ? bDist : bMax;
	}

	// 1-D intersection test
	float longSpan = std::max(aMax, bMax) - std::min(aMin, bMin);
	float sumSpan = aMax - aMin + bMax - bMin;
	return longSpan >= sumSpan;
}
float SeperateLength(std::vector<glm::vec3> vertsA, std::vector<glm::vec3> vertsB, glm::vec3 axis) {
	if (axis == glm::vec3(0)) {
		return false;
	}

	float aMin = std::numeric_limits<float>::max();
	float aMax = std::numeric_limits<float>::min();
	float bMin = std::numeric_limits<float>::max();
	float bMax = std::numeric_limits<float>::min();

	// Calculate interval a's min/max
	for (int i = 0; i < vertsA.size(); i++) {
		float aDist = glm::dot(vertsA[i], glm::normalize(axis));
		aMin = aDist < aMin ? aDist : aMin;
		aMax = aDist > aMax ? aDist : aMax;
	}
	// Calculate interval b's min/max
	for (int i = 0; i < vertsB.size(); i++) {
		float bDist = glm::dot(vertsB[i], glm::normalize(axis));
		bMin = bDist < bMin ? bDist : bMin;
		bMax = bDist > bMax ? bDist : bMax;
	}

	// 1-D intersection test
	float longSpan = std::max(aMax, bMax) - std::min(aMin, bMin);
	float sumSpan = aMax - aMin + bMax - bMin;
	return longSpan - sumSpan;
}

MTV Intersects(RigidBody& a, RigidBody& b) {
	MTV mtv(glm::vec3(0), 0);
	float overlap = std::numeric_limits<float>::max();
	glm::vec3 seperateVector = glm::vec3(0);
	// Loop through all axis for rigid body a
	for (int n = 0; n < a.normals.size(); n++)
	{
		float seperateLength = SeperateLength(a.verts, b.verts, a.normals[n]);
		if (seperateLength >= 0) {
			mtv.intersect = false;
			return mtv;
		}
		else {
			if (std::abs(seperateLength) < overlap) {
				overlap = std::abs(seperateLength);
				seperateVector = a.normals[n];
				mtv.overlap = overlap;
				mtv.vector = seperateVector;
			}
		}
	}
	// Loop through all axis for rigid body b
	for (int n = 0; n < a.normals.size(); n++)
	{
		float seperateLength = SeperateLength(a.verts, b.verts, b.normals[n]);
		if (seperateLength >= 0) {
			mtv.intersect = false;
			return mtv;
		}
		else {
			if (std::abs(seperateLength) < overlap) {
				overlap = std::abs(seperateLength);
				seperateVector = b.normals[n];
				mtv.overlap = overlap;
				mtv.vector = seperateVector;
			}
		}
	}
	// Reference https://gamedev.stackexchange.com/questions/25543/sat-how-do-i-find-the-penetration-vector
	// Loop through all vertices to find the approximate contact point
	float absDist = std::numeric_limits<float>::max();
	for (int i = 0; i < a.verts.size(); i++) {
		float vertDist = std::abs(glm::dot(a.verts[i], seperateVector));
		if (vertDist < absDist) {
			absDist = vertDist;
			mtv.contactPoint = a.verts[i];
		}
	}
	for (int i = 0; i < b.verts.size(); i++) {
		float vertDist = std::abs(glm::dot(b.verts[i], seperateVector));
		if (vertDist < absDist) {
			absDist = vertDist;
			mtv.contactPoint = b.verts[i];
		}
	}
	mtv.intersect = true;
	return mtv;
}

glm::quat RotationChange(const RigidBody &rigidBody, const MTV& mtv, float sign) {

	glm::vec3 preImpactVel = glm::cross(glm::vec3(rigidBody.angularVelocity.x, rigidBody.angularVelocity.y, rigidBody.angularVelocity.z), mtv.contactPoint - rigidBody.position) + rigidBody.velocity;
	preImpactVel = -glm::dot(preImpactVel, mtv.vector * sign) * mtv.vector * sign;

	glm::vec3 relativePoint_a = mtv.contactPoint - rigidBody.position;
	glm::vec3 tourque_a = glm::cross(relativePoint_a, mtv.vector * sign);
	glm::mat3 rotationMat_a = glm::toMat3(rigidBody.rotation);
	glm::vec3 angularVel_a = rotationMat_a * tourque_a;
	angularVel_a = rigidBody.inverseInertiaTensorMat * angularVel_a;
	angularVel_a = glm::inverse(rotationMat_a) * angularVel_a;
	glm::vec3 linearVel_a = glm::cross(angularVel_a, relativePoint_a);

	float linFromAngImpulse = glm::dot(linearVel_a, mtv.vector * sign);
	float velImpulse = linFromAngImpulse + 1 / rigidBody.mass;

	glm::vec3 impulse = preImpactVel / velImpulse;

	glm::vec3 impulseTorque = glm::cross(relativePoint_a, impulse);
	glm::vec3 rotationChange = rotationMat_a * impulseTorque;
	rotationChange = rigidBody.inverseInertiaTensorMat * rotationChange;
	rotationChange = glm::inverse(rotationMat_a) * rotationChange;
	return glm::quat(0, rotationChange.x, rotationChange.y, rotationChange.z);
}

void Scene::Step() {
	//std::cout << glm::to_string(rigidBodies[0].verts[0]) << std::endl;
	//std::cout << rigidBodies[0].normals.size() << std::endl;

	for (int i = 0; i < rigidBodies.size(); i++)
	{
		//std::cout << glm::to_string(rigidBodies[i].rotation) << std::endl;

		// Update Rigid Body Verts/Normals
		glm::mat4 m_translate = glm::translate(glm::mat4(1.0f), glm::vec3(rigidBodies[i].position.x, rigidBodies[i].position.y, rigidBodies[i].position.z));
		glm::mat4 m_rotation = glm::toMat4(rigidBodies[i].rotation);
		glm::mat4 m_rigidBody = m_translate * m_rotation;

		for (size_t v = 0; v < rigidBodies[i].verts.size(); v++) {
			rigidBodies[i].verts[v] = glm::vec3(m_rigidBody * glm::vec4(rigidBodies[i].convex_hull.verts[v], 1.0f));
		}
		for (size_t n = 0; n < rigidBodies[i].normals.size(); n++) {
			rigidBodies[i].normals[n] = glm::vec3(m_rotation * glm::vec4(rigidBodies[i].convex_hull.normals[n], 1.0f));
		}


		// Add gravity (Intergrade force)
		if (!rigidBodies[i].isStatic) {
			rigidBodies[i].velocity += (rigidBodies[i].force / rigidBodies[i].mass + glm::vec3(0, -9.8f, 0)) * dt * 0.5f;
			rigidBodies[i].angularVelocity *= 0.9999f;
		}

		// Calculate Collision and Apply Impulse
		for (int j = 0; j < rigidBodies.size(); j++)
		{
			if (i != j) {
				// Check collision
				MTV mtv = Intersects(rigidBodies[i], rigidBodies[j]);
				if (mtv.intersect) {
					//std::cout << "intersects at: " << total_t << " s \n";
					// Add impulse

					if (glm::dot(mtv.vector, rigidBodies[j].position - rigidBodies[i].position) < 0) {

						rigidBodies[i].angularVelocity += RotationChange(rigidBodies[i], mtv, 1);
						rigidBodies[j].angularVelocity += RotationChange(rigidBodies[j], mtv, -1);

						rigidBodies[i].velocity += mtv.vector / rigidBodies[i].mass;
						rigidBodies[j].velocity += -mtv.vector / rigidBodies[j].mass;

						if (!rigidBodies[i].isStatic)
							rigidBodies[i].position += mtv.vector * mtv.overlap;
						if (!rigidBodies[j].isStatic)
							rigidBodies[j].position += -mtv.vector * mtv.overlap;
					}
					else {
						rigidBodies[i].angularVelocity += RotationChange(rigidBodies[i], mtv, -1);
						rigidBodies[j].angularVelocity += RotationChange(rigidBodies[j], mtv, 1);

						rigidBodies[i].velocity += -mtv.vector / rigidBodies[i].mass;
						rigidBodies[j].velocity += mtv.vector / rigidBodies[j].mass;

						if (!rigidBodies[i].isStatic)
							rigidBodies[i].position += -mtv.vector * mtv.overlap;
						if (!rigidBodies[j].isStatic)
							rigidBodies[j].position += mtv.vector * mtv.overlap;
					}

				}
			}
		}

		// Intergrade velocity
		if (!rigidBodies[i].isStatic) {
			rigidBodies[i].position += rigidBodies[i].velocity * dt;
			rigidBodies[i].rotation = glm::normalize(rigidBodies[i].rotation + rigidBodies[i].angularVelocity * rigidBodies[i].rotation * dt);
		}

	}

	total_t += dt;
}

void Scene::Reset(Model &model, Model &floor)
{
	Reset(model, model, floor, 2.0f, 2.0f, 3.0f);
}

void Scene::Reset(Model& model, Model& convex, Model& floor, float gap, float size, float height)
{
	// Add objects into scene
	for (int i = -size; i < size; i++)
	{
		for (int j = -height; j < height; j++)
		{
			for (int k = -size; k < size; k++)
			{
				RigidBody b(model, convex, glm::vec3(i * gap, j * gap + 15, k * gap), 3.0f);
				glm::vec3 rand_rot = glm::vec3(Random(0, 1), Random(0, 1), Random(0, 1));
				b.rotation = glm::angleAxis(Random(0, PI), glm::normalize(rand_rot));
				glm::vec3 rand_rot2 = glm::vec3(Random(0, 1), Random(0, 1), Random(0, 1));
				//b.angularVelocity = glm::angleAxis(Random(0, PI), glm::normalize(rand_rot2));
				this->Add(b);
			}
		}
	}
	for (int i = 1; i < 5; i++)
	{
		RigidBody b_floor(floor, glm::vec3(i * 4, i * 2, 0), 1.0f);
		b_floor.setStatic(true);
		this->Add(b_floor);
	}

	for (int i = -1; i < 2; i++) {
		RigidBody b_floor(floor, glm::vec3(-4, 0, i * 15), 1.0f);
		b_floor.setStatic(true);
		this->Add(b_floor);
	}
}

void Scene::SetLighting(float pos[4], float amb[4], float diff[4], float spec[4])
{
	for (int i = 0; i < 4; i++) {
		light_position[i] = pos[i];
		light_ambient[i] = amb[i];
		light_diffuse[i] = diff[i];
		light_specular[i] = spec[i];
	}
}

void Scene::Render(const glm::mat4 m_vp) {
	for (int i = 0; i < rigidBodies.size(); i++)
	{
		glm::mat4 m_translate = glm::translate(glm::mat4(1.0f), glm::vec3(rigidBodies[i].position.x, rigidBodies[i].position.y, rigidBodies[i].position.z));
		glm::mat4 m_rotation = glm::toMat4(rigidBodies[i].rotation);
		glm::mat4 m_rigidBody = m_translate * m_rotation;
		rigidBodies[i].model.setLighting(light_position, light_ambient, light_diffuse, light_specular);
		rigidBodies[i].model.render(m_vp, m_rigidBody, true);
	}
}