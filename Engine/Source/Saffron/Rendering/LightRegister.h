#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/PointLight.h"

namespace Se
{
class LightRegister
{
public:
	LightRegister();

	void RegisterPoint(const Vector3& position, float radius, Color color = Colors::White);

	auto operator[](size_t index) -> PointLight;
	auto operator[](size_t index) const -> const PointLight&;

	auto begin() -> std::vector<PointLight>::iterator;
	auto end() -> std::vector<PointLight>::iterator;

	auto begin() const -> std::vector<PointLight>::const_iterator;
	auto end() const -> std::vector<PointLight>::const_iterator;

public:
	static constexpr uint SmFullWidth = 8196, SmFullHeight = 8196;
	static constexpr uint SmWidth = SmFullWidth / 8, SmHeight = SmFullHeight / 8;

private:
	std::vector<PointLight> _pointLights{};

	uint currentX = 0;
	uint currentY = 0;

	static constexpr uint MaxX = SmFullWidth / SmWidth;
	static constexpr uint MaxY = SmFullHeight / SmHeight;
};
}
