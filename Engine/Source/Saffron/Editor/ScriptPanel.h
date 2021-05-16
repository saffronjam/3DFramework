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
		Path Filepath;

		ScriptStat(String namespaceName, String className, Path path) :
			Full(namespaceName + className),
			Namespace(Move(namespaceName)),
			Class(Move(className)),
			Filepath(Move(path))
		{
		}

		ScriptStat(const ScriptStat& stat) :
			ScriptStat(stat.Namespace, stat.Class, stat.Filepath)
		{
		}
	};

	struct Drop
	{
		size_t StorageIndex;
		ScriptStat* Stat;
	};

public:
	ScriptPanel() = default;
	ScriptPanel(Path path);

	void OnGuiRender();
	void SyncScriptPaths();

	void SetAssetFolderpath(Path folderpath) { _scriptFolderPath = Move(folderpath); }

	const List<ScriptStat>& GetScriptStats() const { return _scriptStats; }

private:
	Path _scriptFolderPath;
	List<ScriptStat> _scriptStats;
	Mutex _filepathMutex;
};
}
