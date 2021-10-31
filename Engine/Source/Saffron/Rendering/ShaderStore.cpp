#include "SaffronPCH.h"
#include "Saffron/Rendering/ShaderStore.h"

namespace Se
{
ShaderStore::ShaderStore() :
	Singleton(this)
{
}

void ShaderStore::Add(const std::shared_ptr<Shader>& shader)
{
	Instance()._store.push_back(shader);
}

auto ShaderStore::GetAll() -> std::vector<std::shared_ptr<Shader>>
{
	std::vector<std::shared_ptr<Shader>> result;

	std::erase_if(Instance()._store, [](const auto & weakShader)
	{
		return weakShader.expired();
	});

	std::ranges::transform(
		Instance()._store,
		std::back_inserter(result),
		[](const auto& weakShader)
		{
			return weakShader.lock();
		}
	);
	return result;
}
}
