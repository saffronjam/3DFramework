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
	
	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
