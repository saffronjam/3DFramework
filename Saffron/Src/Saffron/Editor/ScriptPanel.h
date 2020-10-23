#pragma once

#include "Saffron/Core/FileIOManager.h"

namespace Se
{
class ScriptPanel : public RefCounted
{
public:
	struct ScriptStat
	{
		std::string Full;
		std::string Namespace;
		std::string Class;
		fs::path Path;

		ScriptStat(std::string namespaceName, std::string className, fs::path path)
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
	ScriptPanel(fs::path path);

	void OnGuiRender();
	void SyncScriptPaths();

	const std::vector<ScriptStat> &GetScriptStats() const { return m_ScriptStats; }

private:
	fs::path m_ScriptFolderPath;
	std::vector<ScriptStat> m_ScriptStats;
};
}