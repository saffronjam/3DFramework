#pragma once

#include "Saffron/Math/SaffronMath.h"

namespace Se
{
struct AABB
{
	Vector3f Min, Max;

	AABB() :
		Min(0.0f),
		Max(0.0f)
	{
	}

	AABB(const Vector3f& min, const Vector3f& max) :
		Min(min),
		Max(max)
	{
	}
};
}
