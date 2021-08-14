#pragma once

#include "Saffron/Math/Math.h"

namespace Se
{
static constexpr uint MaxLights = 4;

struct alignas(16) PointLight
{
	Matrix LightTransform;
	Vector3 Position;
	float unused = 1.0f;
};
}
