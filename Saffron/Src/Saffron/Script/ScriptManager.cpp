#include "SaffronPCH.h"

#include "Saffron/Gui/Gui.h"
#include "Saffron/Script/ScriptManager.h"

namespace Se
{

std::filesystem::path ScriptManager::m_ScriptFolderPath;
std::vector<std::filesystem::directory_entry> ScriptManager::m_ScriptPaths;

void ScriptManager::Init(std::filesystem::path path)
{
	m_ScriptFolderPath = std::move(path);
}

void ScriptManager::OnGuiRender()
{
	ImGui::Begin("Scripts");
	for ( auto &dirEntry : m_ScriptPaths )
	{
		ImGui::Text(dirEntry.path().string().c_str());
	}
	ImGui::End();
}

void ScriptManager::SyncScriptPaths()
{
	const size_t noFiles = std::distance(std::filesystem::directory_iterator(m_ScriptFolderPath), std::filesystem::directory_iterator{});

	if ( m_ScriptPaths.size() != noFiles )
	{
		m_ScriptPaths.clear();
		for ( const auto &entry : std::filesystem::directory_iterator(m_ScriptFolderPath) )
		{
			if ( entry.path().extension() == ".cs" )
			{
				m_ScriptPaths.push_back(entry);
			}
		}
	}
}
}

