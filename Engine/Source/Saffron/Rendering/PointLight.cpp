#include "SaffronPCH.h"

#include "Saffron/Rendering/PointLight.h"

namespace Se
{
PointLight::PointLight(const Vector3& position, float radius, Se::Color color):
	Position(position),
	Radius(radius),
	Color(color)
{
}

auto PointLight::CreateShaderStruct() const -> ShaderStructs::PointLight
{
	std::array<Matrix, 6> viewProjs;
	const auto& p = Position;
	const auto proj = Matrix::CreatePerspectiveFieldOfView(Math::Pi / 2.0f, 1.0f, 0.5f, Radius);

	viewProjs[0] = Matrix::CreateLookAt(p, p + Vector3{ 1.0, 0.0, 0.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	viewProjs[1] = Matrix::CreateLookAt(p, p + Vector3{ -1.0, 0.0, 0.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	viewProjs[2] = Matrix::CreateLookAt(p, p + Vector3{ 0.0, 1.0, 0.0 }, Vector3{ 0.0, 0.0, 1.0 }) * proj;
	viewProjs[3] = Matrix::CreateLookAt(p, p + Vector3{ 0.0, -1.0, 0.0 }, Vector3{ 0.0, 0.0, -1.0 }) * proj;
	viewProjs[5] = Matrix::CreateLookAt(p, p + Vector3{ 0.0, 0.0, 1.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	viewProjs[4] = Matrix::CreateLookAt(p, p + Vector3{ 0.0, 0.0, -1.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	for (auto& viewProj : viewProjs)
	{
		viewProj = viewProj.Transpose();
	}
	return ShaderStructs::PointLight{viewProjs, Position, Radius, Color};
}
}
