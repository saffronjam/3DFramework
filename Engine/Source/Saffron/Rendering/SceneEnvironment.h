#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables/TextureCube.h"

namespace Se
{
class SceneEnvironment : public Managed<SceneEnvironment>
{
public:
	static std::shared_ptr<SceneEnvironment> Create(const std::filesystem::path& path);

private:
	std::shared_ptr<TextureCube> _cubeMap;
};
}
