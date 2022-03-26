#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Utils.h"
#include "Saffron/Scene/Entity.h"

namespace Se
{
class EntityPanel
{
public:
	EntityPanel();

	void OnUi();

	void SetEntity(const Entity& entity);

private:
	template <typename ComponentType>
	void RenderComponent(bool canBeRemoved = true);

	template <typename ComponentType>
	void RenderComponent(ComponentType&)
	{
		Debug::Break("No ui render function for component {0}", typeid(ComponentType).name());
	}

private:
	Entity _entity;

	std::shared_ptr<Texture> _settingsIcon;
};

template <typename ComponentType>
void EntityPanel::RenderComponent(bool canBeRemoved)
{
	if (_entity.Has<ComponentType>())
	{
		auto& component = _entity.Get<ComponentType>();

		// NOTE(Peter):
		//	This fixes an issue where the first "+" button would display the "Remove" buttons for ALL components on an Entity.
		//	This is due to ImGui::TreeNodeEx only pushing the id for it's children if it's actually open

		using namespace Ui;
		using namespace Ui::Utils;
		using namespace ImGuiUtils;
		using namespace Draw;

		std::string name = Se::Utils::PascalTypeToString<ComponentType>();

		ImGui::PushID(reinterpret_cast<void*>(typeid(ComponentType).hash_code()));
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		bool open = PropertyGridHeader(name);
		bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
		float lineHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;

		bool resetValues = false;
		bool removeComponent = false;

		ImGui::SameLine(contentRegionAvailable.x - lineHeight);
		if (ImGui::InvisibleButton("##options", ImVec2{lineHeight, lineHeight}) || right_clicked)
		{
			ImGui::OpenPopup("ComponentSettings");
		}
		ButtonImage(
			*_settingsIcon,
			IM_COL32(160, 160, 160, 200),
			IM_COL32(160, 160, 160, 255),
			IM_COL32(160, 160, 160, 150),
			ToSaffronRect(RectExpanded(GetItemRect(), -6.0f, -6.0f))
		);

		if (BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Reset")) resetValues = true;

			if (canBeRemoved)
			{
				if (ImGui::MenuItem("Remove component")) removeComponent = true;
			}

			EndPopup();
		}

		if (open)
		{
			RenderComponent(component);
			ImGui::TreePop();
		}

		if (removeComponent || resetValues) _entity.Remove<ComponentType>();

		if (resetValues) _entity.Add<ComponentType>();

		if (!open) ShiftCursorY(-(ImGui::GetStyle().ItemSpacing.y + 1.0f));

		ImGui::PopID();
	}
}
}
