#include "SaffronPCH.h"

#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
ScriptPanel::ScriptPanel(fs::path path)
	: m_ScriptFolderPath(std::move(path))
{
	SyncScriptPaths();
}

void ScriptPanel::OnGuiRender()
{
	ImGui::Begin("Scripts");

	ImGui::ShowDemoWindow();

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
	const size_t noScripts = FileIOManager::GetFileCount(m_ScriptFolderPath, ".cs");

	if ( noScripts > m_ScriptStats.size() )
	{
		m_ScriptStats.clear();
		auto rawPaths = FileIOManager::GetFiles(m_ScriptFolderPath, ".cs");
		std::for_each(rawPaths.begin(), rawPaths.end(), [&](const fs::directory_entry &entry) mutable
					  {
						  m_ScriptStats.emplace_back("Script", entry.path().stem().string(), entry.path());
					  });
	}
}
}

