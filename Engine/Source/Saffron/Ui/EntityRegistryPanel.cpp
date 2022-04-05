#include "SaffronPCH.h"

#include "Saffron/Scene/Entity.h"
#include "Saffron/Ui/EntityRegistryPanel.h"
#include "Saffron/Ui/Ui.h"

namespace Se
{
EntityRegistryPanel::EntityRegistryPanel(Scene* scene) :
	_scene(scene)
{
}

void EntityRegistryPanel::OnUi()
{
	if (!_scene)
	{
		return;
	}

	ImGui::Begin("Entity Registry");

	if (ImGui::Button("Create Entity"))
	{
		auto newEntity = _scene->CreateEntity("New Entity");
		newEntity.Add<ModelComponent>(Model::Create("Cube.fbx"));
	}

	for (auto&& [enttHandle, name] : _scene->Registry().view<NameComponent>().each())
	{
		auto selectedEntity = _scene->SelectedEntity();
		if (selectedEntity.Id() == static_cast<ulong>(enttHandle))
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		}

		Entity entity{Uuid{static_cast<ulong>(enttHandle)}, _scene};

		const auto id = reinterpret_cast<void*>(static_cast<std::intptr_t>(enttHandle));
		constexpr auto flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		const bool treeViewOpened = ImGui::TreeNodeEx(id, flags, "%s", name.Name.c_str());
		if (ImGui::IsItemClicked())
		{
			_scene->SetSelectedEntity(entity);
		}

		if (treeViewOpened)
		{
			if (entity.Has<ModelComponent>())
			{
				auto& model = entity.Get<ModelComponent>();
				ImGui::Text("%s", model.Model->Name().c_str());
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}
}
