#include "SaffronPCH.h"

#include "Saffron/Ui/EntityPanel.h"

namespace Se
{
EntityPanel::EntityPanel() :
	_settingsIcon(Texture::Create("Assets/Textures/Editor/Icons/gear.png"))
{
}

void EntityPanel::OnUi()
{
	if (!_entity.Valid())
	{
		return;
	}

	ImGui::Begin("Entity Panel");

	RenderComponent<TransformComponent>();
	RenderComponent<ModelComponent>();

	ImGui::End();
}

void EntityPanel::SetEntity(const Entity& entity)
{
	_entity = entity;
}

template <>
void EntityPanel::RenderComponent<ModelComponent>(ModelComponent& component)
{
	ImGui::Text("Model name: %s", component.Model->Name().c_str());
}

template <>
void EntityPanel::RenderComponent<TransformComponent>(TransformComponent& component)
{
	using namespace Ui;
	using namespace Ui::Utils;
	using namespace Draw;

	StyleBinder spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
	StyleBinder padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

	ImGui::BeginTable(
		"transformComponent",
		2,
		ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip
	);
	ImGui::TableSetupColumn("label_column", 0, 100.0f);
	ImGui::TableSetupColumn(
		"value_column",
		ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip,
		ImGui::GetContentRegionAvail().x - 100.0f
	);

	ImGui::TableNextRow();
	if(Vec3Control("Translation", component.Translation))
	{
		int x = 1;
	}

	ImGui::TableNextRow();

	Vector3 rotation = Math::ToDegrees(component.Rotation);
	Vec3Control("Rotation", rotation);
	component.Rotation = Math::ToRadians(rotation);

	ImGui::TableNextRow();
	Vec3Control("Scale", component.Scale, 1.0f);

	ImGui::EndTable();

	ShiftCursorY(-8.0f);
	Underline();

	ShiftCursorY(18.0f);
}
}
