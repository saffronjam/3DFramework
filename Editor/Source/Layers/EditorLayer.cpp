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


	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
