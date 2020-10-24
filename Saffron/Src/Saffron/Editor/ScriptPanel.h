#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace Se
{
class ScriptPanel : public ReferenceCounted
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
			Namespace(std::move(namespaceName)),
			Class(std::move(className)),
			Path(std::move(path))
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
	ScriptPanel(Filepath path);

	void OnGuiRender();
	void SyncScriptPaths();

	const ArrayList<ScriptStat> &GetScriptStats() const { return m_ScriptStats; }

private:
	Filepath m_ScriptFolderPath;
	ArrayList<ScriptStat> m_ScriptStats;
};
}