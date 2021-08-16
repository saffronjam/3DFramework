#include "SaffronPCH.h"

#include <utility>

#include "Saffron/Graphics/Material.h"

namespace Se
{
Material::Material(const std::shared_ptr<class Shader>& shader, std::string name) :
	_name(std::move(name)),
	_shader(shader),
	_materialCBuf(ConstantBuffer<MaterialDataCBuf>::Create(3))
{
}

auto Material::Shader() -> Se::Shader&
{
	return *_shader;
}

auto Material::Shader() const -> const Se::Shader&
{
	return *_shader;
}

auto Material::CBuffer() -> ConstantBuffer<MaterialDataCBuf>&
{
	return *_materialCBuf;
}

auto Material::CBuffer() const -> const ConstantBuffer<MaterialDataCBuf>&
{
	return *_materialCBuf;
}

auto Material::Index() const -> uint
{
	return _index;
}

void Material::SetIndex(uint index)
{
	_index = index;
}

void Material::SetMaterialData(const MaterialDataCBuf& materialData)
{
	_materialCBuf->Update(materialData);
}

auto Material::Create(const std::shared_ptr<class Shader>& shader, const std::string& name) -> std::shared_ptr<Material>
{
	return std::make_shared<Material>(shader, name);
}
}
