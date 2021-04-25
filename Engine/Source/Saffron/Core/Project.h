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
	explicit Project(Filepath filepath);
	Project(String name, DateTime lastOpened, Filepath projectFilepath, ArrayList<Filepath> sceneFilepaths);
	
	bool operator==(const Project& rhs) const { return _uuid == rhs._uuid; }

	const Filepath& AddScene(Filepath filepath);
	const Shared<EditorScene>& AddCachedScene(const Shared<EditorScene>& editorScene);

	const DateTime& LastOpened() const { return _lastOpened; }

	void SyncLastOpened() { _lastOpened = DateTime(); }

	UUID GetUUID() const { return _uuid; }

	const String& GetName() const { return _name; }

	const Filepath& GetProjectFolderpath() const { return _projectFolderpath; }

	const Filepath& GetProjectFilepath() const { return _projectFilepath; }

	const ArrayList<Filepath>& GetSceneFilepaths() const { return _sceneFilepaths; }

	const ArrayList<Shared<EditorScene>>& GetSceneCache() const;
	const Filepath GetFullSceneFilepath(const Filepath& relativeFilepath);
	Optional<Shared<EditorScene>> GetCachedScene(const Filepath& filepath);

	bool IsValid() const;
	bool IsValidSceneFilepath(const Filepath& filepath);
	static bool IsValidProjectFilepath(const Filepath& filepath);

private:
	Project(UUID uuid, String name, DateTime lastOpened, Filepath projectFolderpath, Filepath projectFilepath,
	        ArrayList<Filepath> sceneFilepaths);

private:
	UUID _uuid;
	String _name;
	DateTime _lastOpened;

	Filepath _projectFolderpath;
	Filepath _projectFilepath;
	ArrayList<Filepath> _sceneFilepaths;
	ArrayList<Shared<EditorScene>> _sceneCache;
};
}
