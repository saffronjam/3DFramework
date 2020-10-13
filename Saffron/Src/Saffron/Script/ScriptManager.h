#pragma once

#include <filesystem>

namespace Se
{
class ScriptManager
{
public:
	static void Init(std::filesystem::path path);

	static void OnGuiRender();

	static void SyncScriptPaths();
private:
	static std::filesystem::path m_ScriptFolderPath;
	static std::vector<std::filesystem::directory_entry> m_ScriptPaths;


};
}