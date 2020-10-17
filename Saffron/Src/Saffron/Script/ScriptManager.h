#pragma once

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class ScriptManager
{
public:
	struct ScriptName
	{
		std::string Full;
		std::string Namespace;
		std::string Class;

		ScriptName(std::string namespaceName, std::string className)
			:Full(namespaceName + className),
			Namespace(std::move(namespaceName)),
			Class(std::move(className))
		{
		}
	};

	struct Drop
	{
		size_t StorageIndex;
		fs::path *Path;
	};

public:
	static void Init(fs::path path);
	static void OnGuiRender();
	static void SyncScriptPaths();

	static const std::vector<ScriptName> &GetScriptNames() { return m_ScriptNames; }
	static bool CreateScript(const std::string &namespaceName, const std::string &className);

private:
	static fs::path m_ScriptFolderPath;
	static std::vector<fs::directory_entry> m_ScriptPaths;
	static std::vector<ScriptName> m_ScriptNames;

	static std::map<std::string, Ref<Texture2D>> m_TexStore;

};
}