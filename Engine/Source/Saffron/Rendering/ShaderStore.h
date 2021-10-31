#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Se
{
class ShaderStore : public Singleton<ShaderStore>
{
public:
	ShaderStore();

	static void Add(const std::shared_ptr<Shader>& shader);
	static auto GetAll() -> std::vector<std::shared_ptr<Shader>>;

private:
	std::vector<std::weak_ptr<Shader>> _store{};
};
}
