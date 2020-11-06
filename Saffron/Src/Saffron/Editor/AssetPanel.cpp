#include "SaffronPCH.h"

#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Editor/AssetPanel.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
AssetPanel::AssetPanel(Filepath path)
	: m_AssetFolderPath(Move(path))
{
}

void AssetPanel::OnGuiRender()
{
	ScopedLock scopedLock(m_FilepathMutex);
	ImGui::Begin("Assets");

	const int noCollums = std::max(1, static_cast<int>(ImGui::GetContentRegionAvailWidth() / 100.0f));

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * static_cast<float>(noCollums));
	ImGui::Columns(noCollums, nullptr, false);

	for ( size_t i = 0; i < m_AssetStats.size(); i++ )
	{
		ImGui::Button(m_AssetStats[i].Full.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - 5, 60));

		if ( ImGui::BeginDragDropSource(ImGuiDragDropFlags_None) )
		{
			Drop drop = { i, new AssetStat(m_AssetStats[i]) };
			ImGui::SetDragDropPayload("ASSETMGR_DND", &drop, sizeof(Drop));
			ImGui::Text("%s", m_AssetStats[i].Full.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void AssetPanel::SyncAssetPaths()
{
	ScopedLock scopedLock(m_FilepathMutex);
	const size_t noAssets = FileIOManager::GetFileCount(m_AssetFolderPath, ".fbx");

	m_AssetStats.clear();
	auto rawPaths = FileIOManager::GetFiles(m_AssetFolderPath, ".fbx");
	std::for_each(rawPaths.begin(), rawPaths.end(), [&](const DirectoryEntry &entry) mutable
				  {
					  m_AssetStats.emplace_back(entry.path().stem().string(), entry.path().extension().string(), entry.path());
				  });
}
}
