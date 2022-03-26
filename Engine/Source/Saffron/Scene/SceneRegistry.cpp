#include "SaffronPCH.h"

#include "Saffron/Scene/SceneRegistry.h"

namespace Se
{
SceneRegistry::SceneRegistry() :
	Singleton(this)
{
}

auto SceneRegistry::CreateScene() -> const std::shared_ptr<Scene>&
{
	auto scene = std::shared_ptr<Scene>(new Scene());
	Instance()._scenes.emplace(scene->Id(), scene);
	return scene;
}

auto SceneRegistry::GetById(Uuid id) -> const std::shared_ptr<Scene>&
{
	const auto& inst = Instance();
	Debug::Assert(inst._scenes.contains(id));
	return inst._scenes.at(id);
}
}
