#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/DateTime.h"

namespace Se
{
class EditorScene;

class Project : public MemManaged<Project>
{
	friend class ProjectSerializer;
public:
	explicit Project(Filepath filepath);
	Project(String name, DateTime lastOpened, Filepath projectFilepath, ArrayList<Filepath> sceneFilepaths);

	bool operator==(const Project &rhs) const { return m_UUID == rhs.m_UUID; }

	const Filepath &AddScene(Filepath filepath);
	const Shared<EditorScene> &AddCachedScene(const Shared<EditorScene> &editorScene);

	const DateTime &LastOpened()const { return m_LastOpened; }
	void SyncLastOpened() { m_LastOpened = DateTime(); }

	UUID GetUUID() const { return m_UUID; }
	const String &GetName() const { return m_Name; }
	const Filepath &GetProjectFolderpath() const { return m_ProjectFolderpath; }
	const Filepath &GetProjectFilepath() const { return m_ProjectFilepath; }
	const ArrayList<Filepath> &GetSceneFilepaths() const { return m_SceneFilepaths; }
	const ArrayList<Shared<EditorScene>> &GetSceneCache() const;
	const Filepath GetFullSceneFilepath(const Filepath &relativeFilepath);
	Optional<Shared<EditorScene>> GetCachedScene(const Filepath &filepath);

	bool IsValid() const;
	bool IsValidSceneFilepath(const Filepath &filepath);
	static bool IsValidProjectFilepath(const Filepath &filepath);

private:
	Project(UUID uuid, String name, DateTime lastOpened, Filepath projectFolderpath, Filepath projectFilepath, ArrayList<Filepath> sceneFilepaths);

private:
	UUID m_UUID;
	String m_Name;
	DateTime m_LastOpened;

	Filepath m_ProjectFolderpath;
	Filepath m_ProjectFilepath;
	ArrayList<Filepath> m_SceneFilepaths;
	ArrayList<Shared<EditorScene>> m_SceneCache;
};
}