#include "SaffronPCH.h"

#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{
ScriptPanel::ScriptPanel(Path path) :
	_scriptFolderPath(Move(path))
{
	SyncScriptPaths();
}

void ScriptPanel::OnGuiRender()
{
	ScopedLock scopedLock(_filepathMutex);
	ImGui::Begin("Scripts");

	const int noCollums = std::max(1, static_cast<int>(ImGui::GetContentRegionAvailWidth() / 100.0f));

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * static_cast<float>(noCollums));
	ImGui::Columns(noCollums, nullptr, false);

	for (size_t i = 0; i < _scriptStats.size(); i++)
	{
		ImGui::Button(_scriptStats[i].Class.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth() - 5, 60));

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			Drop drop = {i, new ScriptStat(_scriptStats[i])};
			ImGui::SetDragDropPayload("SCRIPTMGR_DND", &drop, sizeof(Drop));
			ImGui::Text("%s", _scriptStats[i].Class.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void ScriptPanel::SyncScriptPaths()
{
	ScopedLock scopedLock(_filepathMutex);
	if (_scriptFolderPath.empty())
	{
		return;
	}

	_scriptStats.clear();
	auto rawPaths = FileIOManager::GetFiles(_scriptFolderPath, ".cs");
	std::for_each(rawPaths.begin(), rawPaths.end(), [&](const DirEntry& entry) mutable
	{
		_scriptStats.emplace_back("Script", entry.path().stem().string(), entry.path());
	});
}
}
