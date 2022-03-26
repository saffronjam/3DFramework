#include "Layers/EditorLayer.h"

#include "Saffron/Scene/Entity.h"

namespace Se
{
EditorLayer::EditorLayer() :
	_scene(SceneSerializer::ReadFromFile("development.json")),
	_viewportPanel("Output"),
	_entityRegistryPanel(_scene.get())
{
}

void EditorLayer::OnAttach()
{
	const auto& window = App::Instance().Window();
	_scene->SetViewportSize(window.Width(), window.Height());

	_scene->ViewportResized += [this](const SizeEvent& event)
	{
		_viewportPanel.SetImage(_scene->Renderer().FinalTargetPtr()->ImagePtrByIndex(0));
		_depthViewportPanel.SetImage(_scene->Renderer().FinalTargetPtr()->DepthImagePtr());
		return false;
	};
	_viewportPanel.Resized += [this](const SizeEvent& event)
	{
		_scene->SetViewportSize(event.Width, event.Height);
		return false;
	};

	_viewportPanel.Renderered += [this](const SizeEvent& event)
	{
		if (!_scene->HasSelectedEntity())
		{
			return false;
		}

		auto selected = _scene->SelectedEntity();
		auto& transformComponent = selected.Get<TransformComponent>();
		auto transform = transformComponent.Transform();
		auto& camera = _scene->Camera();

		Ui::BeginGizmo(event.Width, event.Height);
		Ui::Gizmo(transform, camera.View(), camera.Projection(), _gizmoControl);

		Vector3 translation, scale;
		Quaternion rotation;
		if (transform.Decompose(scale, rotation, translation))
		{
			transformComponent.Scale = scale;
			transformComponent.Translation = translation;
			transformComponent.Rotation = Math::ToEulerAngles(rotation);
		}
		return false;
	};

	_scene->OnSelcetedEntity += [this](const Entity& entity)
	{
		_entityPanel.SetEntity(entity);
		return false;
	};

	App::Instance().Window().GainedFocus += []
	{
		for (auto& shader : ShaderStore::GetAll())
		{
			shader->Reload();
		}
		return false;
	};
}

void EditorLayer::OnDetach()
{
	SceneSerializer::WriteToFile(*_scene, "development.json");
}

void EditorLayer::OnUpdate(TimeSpan ts)
{
	_scene->OnUpdate(ts);
	_scene->OnRender();

	if (Keyboard::IsKeyPressed(KeyCode::Num1))
	{
		_gizmoControl = GizmoControl::Translate;
	}
	if (Keyboard::IsKeyPressed(KeyCode::Num2))
	{
		_gizmoControl = GizmoControl::Rotate;
	}
	if (Keyboard::IsKeyPressed(KeyCode::Num3))
	{
		_gizmoControl = GizmoControl::Scale;
	}
}

void EditorLayer::OnUi()
{
	_dockSpacePanel.Begin();

	_viewportPanel.OnUi();
	_depthViewportPanel.OnUi();
	_entityPanel.OnUi();
	_entityRegistryPanel.OnUi();
	_scene->OnUi();

	auto shaders = ShaderStore::GetAll();

	ImGui::Begin("Shaders");
	for (auto& shader : shaders)
	{
		ImGui::Text(shader->Name().c_str());
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			shader->Reload();
		}
	}
	ImGui::End();

	ImGui::Begin("Editor");
	if (ImGui::RadioButton("Translate", _gizmoControl == GizmoControl::Translate))
	{
		_gizmoControl = GizmoControl::Translate;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", _gizmoControl == GizmoControl::Rotate))
	{
		_gizmoControl = GizmoControl::Rotate;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", _gizmoControl == GizmoControl::Scale))
	{
		_gizmoControl = GizmoControl::Scale;
	}
	ImGui::End();

	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
