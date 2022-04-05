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

	
	const auto proj = glm::perspectiveFov(Math::Pi / 2.0f, 1.0f, 1.0f, 0.5f, Radius);

	viewProjs[0] = glm::lookAt(p, p + Vector3{ 1.0, 0.0, 0.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	viewProjs[1] = glm::lookAt(p, p + Vector3{ -1.0, 0.0, 0.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	viewProjs[2] = glm::lookAt(p, p + Vector3{ 0.0, 1.0, 0.0 }, Vector3{ 0.0, 0.0, 1.0 }) * proj;
	viewProjs[3] = glm::lookAt(p, p + Vector3{ 0.0, -1.0, 0.0 }, Vector3{ 0.0, 0.0, -1.0 }) * proj;
	viewProjs[5] = glm::lookAt(p, p + Vector3{ 0.0, 0.0, 1.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	viewProjs[4] = glm::lookAt(p, p + Vector3{ 0.0, 0.0, -1.0 }, Vector3{ 0.0, 1.0, 0.0 }) * proj;
	
	return ShaderStructs::PointLight{viewProjs, Position, Radius, Color};
}
}
