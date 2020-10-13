#pragma once

#include <filesystem>

#include "Saffron/Renderer/Texture.h"

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
	static std::vector<std::string> m_ScriptNames;

	static std::map<std::string, Ref<Texture2D>> m_TexStore;

};
}