﻿#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Enums.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Assimp
{
class Importer;
}

struct aiScene;
struct aiMaterial;

namespace Se
{
class Model;
class Material;
class Texture;

class ModelRegistry : public Singleton<ModelRegistry>
{
	using MatTexContainer = std::map<ModelTextureMapType, std::shared_ptr<Texture>>;
public:
	ModelRegistry();
	~ModelRegistry() override;

	auto Import(const std::filesystem::path& path) -> std::shared_ptr<Model>;

private:
	auto ImportMaterials(
		const aiScene& scene,
		const std::filesystem::path& fullpath
	) const -> std::tuple<std::vector<std::shared_ptr<Material>>, std::vector<MatTexContainer>>;
	auto ImportMaterial(
		aiMaterial& aiMaterial,
		const std::filesystem::path& fullpath
	) const -> std::tuple<std::shared_ptr<Material>, MatTexContainer>;

	static auto CreateDefaultMaterial() -> std::shared_ptr<Material>;
private:
	Assimp::Importer* _importer;
	std::unordered_map<std::string, std::shared_ptr<Bindable>> _meshes;
	std::shared_ptr<Shader> _modelShader;

	static const uint DefaultImportFlags;
	static const std::filesystem::path BasePath;
};
}
