#pragma once

#include "Saffron/Math/Math.h"

namespace Se
{
static constexpr uint MaxLights = 4;

namespace ShaderStructs
{
struct alignas(16) PointLight
{
	PointLight() = default;

	PointLight(const Vector3& position, float radius, Color color = Colors::White) :
		Position(position),
		Radius(radius),
		Color(color)
	{
		const auto& pos = Position;
		const auto proj = Matrix::CreatePerspectiveFieldOfView(
			Math::Pi / 2.0f,
			1.0f,
			0.5f,
			Radius
		);
		
		LookAt[0] = Matrix::CreateLookAt(pos, pos + Vector3{ 1.0, 0.0, 0.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
		LookAt[1] = Matrix::CreateLookAt(pos, pos + Vector3{ -1.0, 0.0, 0.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
		LookAt[2] = Matrix::CreateLookAt(pos, pos + Vector3{ 0.0, 1.0, 0.0 }, Vector3{ 0.0, 0.0, 1.0 }) * proj;
		LookAt[3] = Matrix::CreateLookAt(pos, pos + Vector3{ 0.0, -1.0, 0.0 }, Vector3{ 0.0, 0.0, -1.0 }) * proj;
		LookAt[5] = Matrix::CreateLookAt(pos, pos + Vector3{ 0.0, 0.0, 1.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
		LookAt[4] = Matrix::CreateLookAt(pos, pos + Vector3{ 0.0, 0.0, -1.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	}

	std::array<Matrix, 6> LookAt;
	Vector3 Position;
	float Radius;
	Color Color;
};
}
}
