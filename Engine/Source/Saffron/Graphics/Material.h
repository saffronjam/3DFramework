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

	bool UseNormalMap;
};


class Material
{
public:
	Material(const std::shared_ptr<Shader>& shader, std::string name);

	auto Shader() -> Shader&;
	auto Shader() const -> const class Shader&;
	auto CBuffer() -> ConstantBuffer<MaterialDataCBuf>&;
	auto CBuffer() const -> const ConstantBuffer<MaterialDataCBuf>&;

	auto Index() const -> uint;
	void SetIndex(uint index);

	void SetMaterialData(const MaterialDataCBuf& materialData);

	static auto Create(
		const std::shared_ptr<class Shader>& shader,
		const std::string& name
	) -> std::shared_ptr<Material>;

private:
	std::string _name;
	std::shared_ptr<class Shader> _shader;
	std::shared_ptr<ConstantBuffer<MaterialDataCBuf>> _materialCBuf;

	uint _index = 0;
};
}
