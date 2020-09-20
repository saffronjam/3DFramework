#pragma once

#include "Saffron/Core/Math/AABB.h"
#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
struct Ray
{
	glm::vec3 Origin{}, Direction{};

	Ray(const glm::vec3 &origin, const glm::vec3 &direction);

	static Ray Zero();

	bool IntersectsAABB(const AABB &aabb, float &t) const;
	bool IntersectsTriangle(const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C, float &t) const;
};
}