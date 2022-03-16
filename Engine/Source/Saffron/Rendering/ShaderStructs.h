#pragma once

#include "Saffron/Math/Math.h"

namespace Se
{
static constexpr uint MaxLights = 4;

namespace ShaderStructs
{
struct alignas(16) PointLight
{
	std::array<Matrix, 6> LookAt;
	Vector3 Position;
	float Radius;
	Color Color;
};
}
}
