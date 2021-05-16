#include "SaffronPCH.h"

#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Editor/AssetPanel.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
AssetPanel::AssetPanel(Path path) :
	_assetFolderPath(Move(path))
{
}

void AssetPanel::OnGuiRender()
{
	ScopedLock scopedLock(_filepathMutex);
	ImGui::Begin("Assets");

	const int noCollums = std::max(1, static_cast<int>(ImGui::GetContentRegionAvailWidth() / 100.0f));

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * static_cast<float>(noCollums));
	ImGui::Columns(noCollums, nullptr, false);

	for (size_t i = 0; i < _assetStats.size(); i++)
	{
		ImGui::Button(_assetStats[i].Full.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - 5, 60));

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			Drop drop = {i, new AssetStat(_assetStats[i])};
			ImGui::SetDragDropPayload("ASSETMGR_DND", &drop, sizeof(Drop));
			ImGui::Text("%s", _assetStats[i].Full.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void AssetPanel::SyncAssetPaths()
{
	ScopedLock scopedLock(_filepathMutex);
	if (_assetFolderPath.empty())
	{
		return;
	}

	_assetStats.clear();
	auto rawPaths = FileIOManager::GetFiles(_assetFolderPath, ".fbx");
	std::for_each(rawPaths.begin(), rawPaths.end(), [&](const DirEntry& entry) mutable
	{
		_assetStats.emplace_back(entry.path().stem().string(), entry.path().extension().string(), entry.path());
	});
}
}
