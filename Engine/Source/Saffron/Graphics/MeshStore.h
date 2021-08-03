#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Assimp
{
class Importer;
}

namespace Se
{
class Mesh;

class MeshStore : public Singleton<MeshStore>
{
public:
	MeshStore();
	~MeshStore() override;

	auto Import(const std::filesystem::path& path) -> std::shared_ptr<Mesh>;

private:
	Assimp::Importer* _importer;
	std::unordered_map<std::string, std::shared_ptr<Bindable>> _meshes;
	
	static const uint DefaultImportFlags;
	static const std::filesystem::path BasePath;
};
}
