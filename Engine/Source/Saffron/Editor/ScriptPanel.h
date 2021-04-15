#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace Se
{
class ScriptPanel : public Managed
{
public:
	struct ScriptStat
	{
		String Full;
		String Namespace;
		String Class;
		Filepath Path;

		ScriptStat(String namespaceName, String className, Filepath path)
			:Full(namespaceName + className),
			Namespace(Move(namespaceName)),
			Class(Move(className)),
			Path(Move(path))
		{
		}
		ScriptStat(const ScriptStat &stat)
			:ScriptStat(stat.Namespace, stat.Class, stat.Path)
		{
		}
	};

	struct Drop
	{
		size_t StorageIndex;
		ScriptStat *Stat;
	};

public:
	ScriptPanel() = default;
	ScriptPanel(Filepath path);

	void OnGuiRender();
	void SyncScriptPaths();

	void SetAssetFolderpath(Filepath folderpath) { m_ScriptFolderPath = Move(folderpath); }
	const ArrayList<ScriptStat> &GetScriptStats() const { return m_ScriptStats; }

private:
	Filepath m_ScriptFolderPath;
	ArrayList<ScriptStat> m_ScriptStats;
	Mutex m_FilepathMutex;
};
}