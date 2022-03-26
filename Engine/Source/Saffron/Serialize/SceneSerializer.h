#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Scene;

class SceneSerializer
{
public:
	static void WriteToFile(const Scene& scene, const std::filesystem::path& output);

	static auto ReadFromFile(const std::filesystem::path& path) -> std::shared_ptr<Scene>;

private:
	static const std::filesystem::path FromBuildPath;
	static const std::filesystem::path BasePath;
};
}
