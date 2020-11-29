#pragma once
#include <glm/glm.hpp>

struct MTV {
	glm::vec3 vector;
	float overlap;
	bool intersect;
	glm::vec3 contactPoint;

	MTV(glm::vec3 _vector, float _overlap) {
		vector = _vector;
		overlap = _overlap;
		intersect = false;
		contactPoint = glm::vec3(0);
	}
};