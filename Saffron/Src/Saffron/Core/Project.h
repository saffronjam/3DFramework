#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/DateTime.h"

namespace Se
{
class EditorScene;

class Project : public ReferenceCounted
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
	const Filepath &GetProjectFilepath() const { return m_ProjectFilepath; }
	const ArrayList<Filepath> &GetSceneFilepaths() const { return m_SceneFilepaths; }
	const ArrayList<Shared<EditorScene>> &GetSceneCache() const;

	bool IsValid() const;
	static bool IsValidFilepath(const Filepath &filepath);

private:
	Project(UUID mUuid, String mName, DateTime mLastOpened, Filepath mProjectFilepath, ArrayList<Filepath> mSceneFilepaths);

private:
	UUID m_UUID;
	String m_Name;
	DateTime m_LastOpened;

	Filepath m_ProjectFilepath;
	ArrayList<Filepath> m_SceneFilepaths;
	ArrayList<Shared<EditorScene>> m_SceneCache;
};
}