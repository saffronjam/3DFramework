#pragma once

#include "Saffron/Math/AABB.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
struct Ray
{
	Vector3 Origin{}, Direction{};

	Ray(const Vector3& origin, const Vector3& direction);

	static Ray Zero();

	bool IntersectsAABB(const AABB& aabb, float& t) const;
	bool IntersectsTriangle(const Vector3& A, const Vector3& B, const Vector3& C, float& t) const;
};
}
