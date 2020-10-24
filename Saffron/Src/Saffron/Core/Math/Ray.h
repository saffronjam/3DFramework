#pragma once

#include "Saffron/Core/Math/AABB.h"
#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
struct Ray
{
	Vector3f Origin{}, Direction{};

	Ray(const Vector3f &origin, const Vector3f &direction);

	static Ray Zero();

	bool IntersectsAABB(const AABB &aabb, float &t) const;
	bool IntersectsTriangle(const Vector3f &A, const Vector3f &B, const Vector3f &C, float &t) const;
};
}