#include "SaffronPCH.h"

#include "Saffron/Rendering/LightRegister.h"

namespace Se
{
LightRegister::LightRegister() :
	_atlas(Framebuffer::Create({SmFullWidth, SmFullHeight, {ImageFormat::RGBA, ImageFormat::Depth}}))
{
}

void LightRegister::RegisterPoint(const Vector3& position, float radius, Color color)
{
	auto& pointLight = _pointLights.emplace_back(position, radius, color);

	for (int i = 0; i < 6; i++)
	{
		float xStart = static_cast<float>(currentX) * static_cast<float>(SmWidth);
		float xEnd = static_cast<float>(currentX + 1) * static_cast<float>(SmWidth);
		float yStart = static_cast<float>(currentY) * static_cast<float>(SmWidth);
		float yEnd = static_cast<float>(currentY + 1) * static_cast<float>(SmWidth);

		pointLight._faces[i].SmTopLeft = Vector2{xStart, yStart};
		pointLight._faces[i].SmBottomRight = Vector2{xEnd, yEnd};

		currentX++;
		currentY++;
	}
}

auto LightRegister::Atlas() const -> const std::shared_ptr<Framebuffer>&
{
	return _atlas;
}

auto LightRegister::operator[](size_t index) -> PointLight
{
	return _pointLights[index];
}

auto LightRegister::operator[](size_t index) const -> const PointLight&
{
	return _pointLights[index];
}

auto LightRegister::begin() -> std::vector<PointLight>::iterator
{
	return _pointLights.begin();
}

auto LightRegister::end() -> std::vector<PointLight>::iterator
{
	return _pointLights.end();
}

auto LightRegister::begin() const -> std::vector<PointLight>::const_iterator
{
	return _pointLights.begin();
}

auto LightRegister::end() const -> std::vector<PointLight>::const_iterator
{
	return _pointLights.end();
}
}
