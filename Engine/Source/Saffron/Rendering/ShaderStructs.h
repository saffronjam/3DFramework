#pragma once

#include "Saffron/Base.h"

namespace Se::ShaderStructs
{
struct PointLight
{
	std::array<Matrix, 6> ViewProjs;
	Vector3 Position;
	float Radius;
	Color Color;
};
}
