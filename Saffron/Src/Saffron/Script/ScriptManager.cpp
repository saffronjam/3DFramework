#include "SaffronPCH.h"

#include "Saffron/Gui/Gui.h"
#include "Saffron/Script/ScriptManager.h"

namespace Se
{

std::filesystem::path ScriptManager::m_ScriptFolderPath;
std::vector<std::filesystem::directory_entry> ScriptManager::m_ScriptPaths;
std::vector<std::string> ScriptManager::m_ScriptNames;
std::map<std::string, Ref<Texture2D>> ScriptManager::m_TexStore;

void ScriptManager::Init(std::filesystem::path path)
{
	m_ScriptFolderPath = std::move(path);

	m_TexStore["ScriptCS"] = Texture2D::Create("Assets/Editor/ScriptCS.png");

	SyncScriptPaths();
}

void ScriptManager::OnGuiRender()
{

	ImGui::Begin("Scripts");


	const int noCollums = std::max(1, static_cast<int>(ImGui::GetContentRegionAvailWidth() / 100.0f));

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * noCollums);
	ImGui::Columns(noCollums, nullptr, false);

	for ( size_t i = 0; i < m_ScriptNames.size(); i++ )
	{
		ImGui::Button(m_ScriptNames[i].c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - 5, 60));

		if ( ImGui::BeginDragDropSource(ImGuiDragDropFlags_None) )
		{
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(size_t));
			ImGui::Text("%s", m_ScriptNames[i].c_str());
			ImGui::EndDragDropSource();
		}
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL") )
			{
				IM_ASSERT(payload->DataSize == sizeof(size_t));
				const auto payloadIndex = *static_cast<const size_t *>(payload->Data);
				std::swap(m_ScriptNames[i], m_ScriptNames[payloadIndex]);
				std::swap(m_ScriptPaths[i], m_ScriptPaths[payloadIndex]);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void ScriptManager::SyncScriptPaths()
{
	const size_t noFiles = std::distance(std::filesystem::directory_iterator(m_ScriptFolderPath), std::filesystem::directory_iterator{});

	if ( m_ScriptPaths.size() != noFiles )
	{
		m_ScriptPaths.clear();
		m_ScriptNames.clear();
		for ( const auto &entry : std::filesystem::directory_iterator(m_ScriptFolderPath) )
		{
			if ( entry.path().extension() == ".cs" )
			{
				m_ScriptPaths.push_back(entry);
				m_ScriptNames.push_back(entry.path().stem().string());
			}
		}
	}
}
}

