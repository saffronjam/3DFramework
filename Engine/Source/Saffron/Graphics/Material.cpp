#include "SaffronPCH.h"

#include <utility>

#include "Saffron/Graphics/Material.h"

namespace Se
{
Material::Material(std::string name) :
	_name(std::move(name))
{
}

auto Material::Index() const -> uint
{
	return _index;
}

void Material::SetIndex(uint index)
{
	_index = index;
}

auto Material::Create(const std::string& name) -> std::shared_ptr<Material>
{
	return std::make_shared<Material>(name);
}
}
