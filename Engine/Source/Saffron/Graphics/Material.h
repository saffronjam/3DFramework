#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Material
{
public:
	Vector3 AlbedoColor;
	float Metalness;
	float Roughness;
	float Emission;

	float EnvMapRotation;

	bool UseNormalMap;
};
}
