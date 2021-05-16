#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/DateTime.h"

namespace Se
{
class EditorScene;

class Project : public Managed
{
	friend class ProjectSerializer;
public:
	explicit Project(Path filepath);
	Project(String name, DateTime lastOpened, Path projectFilepath, List<Path> sceneFilepaths);
	
	bool operator==(const Project& rhs) const { return _uuid == rhs._uuid; }

	const Path& AddScene(Path filepath);
	const Shared<EditorScene>& AddCachedScene(const Shared<EditorScene>& editorScene);

	const DateTime& LastOpened() const { return _lastOpened; }

	void SyncLastOpened() { _lastOpened = DateTime(); }

	UUID GetUUID() const { return _uuid; }

	const String& GetName() const { return _name; }

	const Path& GetProjectFolderpath() const { return _projectFolderpath; }

	const Path& GetProjectFilepath() const { return _projectFilepath; }

	const List<Path>& GetSceneFilepaths() const { return _sceneFilepaths; }

	const List<Shared<EditorScene>>& GetSceneCache() const;
	const Path GetFullSceneFilepath(const Path& relativeFilepath);
	Optional<Shared<EditorScene>> GetCachedScene(const Path& filepath);

	bool IsValid() const;
	bool IsValidSceneFilepath(const Path& filepath);
	static bool IsValidProjectFilepath(const Path& filepath);

private:
	Project(UUID uuid, String name, DateTime lastOpened, Path projectFolderpath, Path projectFilepath,
	        List<Path> sceneFilepaths);

private:
	UUID _uuid;
	String _name;
	DateTime _lastOpened;

	Path _projectFolderpath;
	Path _projectFilepath;
	List<Path> _sceneFilepaths;
	List<Shared<EditorScene>> _sceneCache;
};
}
