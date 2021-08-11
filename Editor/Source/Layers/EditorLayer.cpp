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
		_camera.SetProjection(
			Matrix::CreatePerspectiveFieldOfView(
				Math::Pi / 4.0f,
				static_cast<float>(event.Width) / static_cast<float>(event.Height),
				0.1f,
				100.0f
			)
		);
		return false;
	};
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(TimeSpan ts)
{
	_camera.OnUpdate(ts);

	_scene.OnUpdate(ts);
	_scene.OnRender(_camera.Data());
}

void EditorLayer::OnUi()
{
	_dockSpacePanel.Begin();

	_viewportPanel.OnUi();
	_depthViewportPanel.OnUi();

	_camera.OnUi();

	ImGui::Begin("Texture");

	//ImGui::Image(&_mesh->Textures()[0]->ShaderView(), {500, 500});

	ImGui::End();

	_scene.OnUi();


	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
