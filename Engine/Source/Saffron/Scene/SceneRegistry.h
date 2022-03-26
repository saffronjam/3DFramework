#pragma once

#include "Saffron/Base.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class SceneRegistry : Singleton<SceneRegistry>
{
public:
	SceneRegistry();

	static auto CreateScene() -> const std::shared_ptr<Scene>&;
	static auto GetById(Uuid id) -> const std::shared_ptr<Scene>&;

private:
	std::map<Uuid, std::shared_ptr<Scene>> _scenes{};
};
}
