#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables.h"

namespace Se
{
struct alignas(16) MaterialDataCBuf
{
	Color AlbedoColor;
	float Metalness;
	float Roughness;
	float Emission;
	float EnvMapRotation;

	int UseNormalMap;
};


class Material
{
public:
	Material(std::string name);

	auto Index() const -> uint;
	void SetIndex(uint index);

	static auto Create(const std::string& name) -> std::shared_ptr<Material>;

private:
	std::string _name;

	uint _index = 0;
};
}
