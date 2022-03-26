#pragma once

#include "Saffron/Base.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class EntityRegistryPanel
{
public:
	explicit EntityRegistryPanel(Scene* scene);

	void OnUi();

private:
	Scene* _scene = nullptr;
};
}
