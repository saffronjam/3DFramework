#pragma once

#include "Saffron/Base.h"
#include "Saffron/Math/Math.h"
#include "Saffron/Rendering/ShaderStructs.h"

namespace Se
{
static constexpr uint MaxLights = 4;

class alignas(16) PointLight
{
private:
	struct alignas(16) FaceData
	{
		Matrix ViewProj;
		Vector2 SmTopLeft;
		Vector2 SmBottomRight;
	};

public:
	PointLight() = default;

	PointLight(const Vector3& position, float radius, Color color = Colors::White);

	auto CreateShaderStruct() const -> ShaderStructs::PointLight;

	Vector3 Position;
	float Radius = 0.0f;
	Color Color;

private:
	std::array<FaceData, 6> _faces{};

private:
	friend class LightRegister;
};
}
