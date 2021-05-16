#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Project.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Serialize/ProjectSerializer.h"

namespace Se
{
Project::Project(Path filepath)
{
	const auto invalidateThis = [this]
	{
		_uuid = 0ull;
		_name = "";
		_projectFilepath = "";
		_sceneFilepaths = {};
		_sceneCache = {};
	};

	if (IsValidProjectFilepath(filepath))
	{
		ProjectSerializer serializer(*this);
		if (!serializer.Deserialize(filepath))
		{
			invalidateThis();
		}
	}
	else
	{
		invalidateThis();
	}
}

Project::Project(String name, DateTime lastOpened, Path projectFilepath, List<Path> sceneFilepaths) :
	_name(std::move(name)),
	_lastOpened(lastOpened),
	_projectFilepath(std::move(projectFilepath)),
	_sceneFilepaths(std::move(sceneFilepaths))
{
}

const Path& Project::AddScene(Path filepath)
{
	auto candidateSceneFilepath = "res/Scenes/" + filepath.stem().string() + filepath.extension().string();

	const auto maybeCopy = std::find(_sceneFilepaths.begin(), _sceneFilepaths.end(), candidateSceneFilepath);
	if (maybeCopy != _sceneFilepaths.end())
	{
		return *maybeCopy;
	}
	_sceneFilepaths.push_back(Move(candidateSceneFilepath));
	return _sceneFilepaths.back();
}

const Shared<EditorScene>& Project::AddCachedScene(const Shared<EditorScene>& editorScene)
{
	_sceneCache.push_back(editorScene);
	return _sceneCache.back();
}

Project::Project(UUID uuid, String name, DateTime lastOpened, Path projectFolderpath, Path projectFilepath,
                 List<Path> sceneFilepaths) :
	_uuid(uuid),
	_name(Move(name)),
	_lastOpened(lastOpened),
	_projectFolderpath(Move(projectFolderpath)),
	_projectFilepath(Move(projectFolderpath)),
	_sceneFilepaths(Move(sceneFilepaths))

{
}

const List<Shared<EditorScene>>& Project::GetSceneCache() const
{
	return _sceneCache;
}

const Path Project::GetFullSceneFilepath(const Path& relativeFilepath)
{
	return _projectFolderpath.string() + relativeFilepath.string();
}

Optional<Shared<EditorScene>> Project::GetCachedScene(const Path& filepath)
{
	return std::nullopt;
}

bool Project::IsValid() const
{
	const bool correctData = !_name.empty() && !_projectFolderpath.empty() && !_projectFilepath.empty() && !
		_sceneFilepaths.empty() && !_sceneFilepaths.front().empty(); // Correct on data level
	const auto projectFullFilepath = _projectFolderpath.string() + _projectFilepath.string();
	bool correctOnDisk = FileIOManager::FileExists(_projectFolderpath) && FileIOManager::FileExists(
		projectFullFilepath);
	for (const auto& sceneFilepath : _sceneFilepaths)
	{
		const auto sceneFullFilepath = _projectFolderpath.string() + sceneFilepath.string();
		if (!FileIOManager::FileExists(sceneFullFilepath))
		{
			correctOnDisk = false;
			break;
		}
	}
	return correctData && correctOnDisk;
}

bool Project::IsValidProjectFilepath(const Path& filepath)
{
	return !filepath.empty() && filepath.extension() == ".spr" && FileIOManager::FileExists(filepath);
}

bool Project::IsValidSceneFilepath(const Path& filepath)
{
	return !filepath.empty() && filepath.extension() == ".ssc" &&
		FileIOManager::FileExists(GetFullSceneFilepath(filepath)) && std::find(
			_sceneFilepaths.begin(), _sceneFilepaths.end(), filepath) != _sceneFilepaths.end();
}
}
