#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Assimp
{
class Importer;
}

namespace Se
{
class Model;

class ModelStore : public Singleton<ModelStore>
{
public:
	ModelStore();
	~ModelStore() override;

	auto Import(const std::filesystem::path& path) -> std::shared_ptr<Model>;

private:
	Assimp::Importer* _importer;
	std::unordered_map<std::string, std::shared_ptr<Bindable>> _meshes;
	std::shared_ptr<Shader> _meshShader;

	static const uint DefaultImportFlags;
	static const std::filesystem::path BasePath;
};
}
