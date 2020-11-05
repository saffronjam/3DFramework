#include "SaffronPCH.h"

#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
ScriptPanel::ScriptPanel(Filepath path)
	: m_ScriptFolderPath(Move(path))
{
	SyncScriptPaths();
}

void ScriptPanel::OnGuiRender()
{
	ScopedLock scopedLock(m_FilepathMutex);
	ImGui::Begin("Scripts");

	const int noCollums = std::max(1, static_cast<int>(ImGui::GetContentRegionAvailWidth() / 100.0f));

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * static_cast<float>(noCollums));
	ImGui::Columns(noCollums, nullptr, false);

	for ( size_t i = 0; i < m_ScriptStats.size(); i++ )
	{
		ImGui::Button(m_ScriptStats[i].Class.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - 5, 60));

		if ( ImGui::BeginDragDropSource(ImGuiDragDropFlags_None) )
		{
			Drop drop = { i, new ScriptStat(m_ScriptStats[i]) };
			ImGui::SetDragDropPayload("SCRIPTMGR_DND", &drop, sizeof(Drop));
			ImGui::Text("%s", m_ScriptStats[i].Class.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void ScriptPanel::SyncScriptPaths()
{
	ScopedLock scopedLock(m_FilepathMutex);
	const size_t noScripts = FileIOManager::GetFileCount(m_ScriptFolderPath, ".cs");

	m_ScriptStats.clear();
	auto rawPaths = FileIOManager::GetFiles(m_ScriptFolderPath, ".cs");
	std::for_each(rawPaths.begin(), rawPaths.end(), [&](const DirectoryEntry &entry) mutable
				  {
					  m_ScriptStats.emplace_back("Script", entry.path().stem().string(), entry.path());
				  });
}
}

