#include "Layers/EditorLayer.h"

namespace Se
{
EditorLayer::EditorLayer() :
	_viewportPanel("Output")
{
}

void EditorLayer::OnAttach()
{
	const auto& window = App::Instance().Window();
	_scene.SetViewportSize(window.Width(), window.Height());

	_scene.ViewportResized += [this](const SizeEvent& event)
	{
		_viewportPanel.SetImage(_scene.Renderer().FinalTargetPtr()->ImagePtrByIndex(0));
		_depthViewportPanel.SetImage(_scene.Renderer().FinalTargetPtr()->DepthImagePtr());
		return false;
	};
	_viewportPanel.Resized += [this](const SizeEvent& event)
	{
		_scene.SetViewportSize(event.Width, event.Height);
		return false;
	};

	_viewportPanel.Renderered += [this](const SizeEvent& event)
	{
		auto& selected = _scene.SelectedModel();
		auto& camera = _scene.Camera();

		Ui::BeginGizmo(event.Width, event.Height);
		Ui::Gizmo(selected->Transform(), camera.View(), camera.Projection(), _gizmoControl);
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
}

void EditorLayer::OnUpdate(TimeSpan ts)
{
	_scene.OnUpdate(ts);
	_scene.OnRender();

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
	_scene.OnUi();

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
