#include "Layers/EditorLayer.h"

namespace Se
{
EditorLayer::EditorLayer() :
	_framebuffer(Framebuffer::Create(FramebufferSpec{500, 500, {ImageFormat::RGBA, ImageFormat::Depth24Stencil8}})),
	_viewport(App::Instance().Window().Width(), App::Instance().Window().Height()),
	_viewportPanel("Output")
{
}

void EditorLayer::OnAttach()
{
	const auto& window = App::Instance().Window();

	struct Vertex
	{
		float x, y, z;
	};

	_mesh = Mesh::Create("Mario.fbx");

	_mesh->Transform() *= Matrix::CreateScale(0.01f);

	_framebuffer->Resized += [this](const SizeEvent& event)
	{
		_viewportPanel.SetImage(_framebuffer->FinalTargetPtr());
		_depthViewportPanel.SetImage(_framebuffer->DepthTargetPtr());
		return false;
	};
	_viewportPanel.Resized += [this](const SizeEvent& event)
	{
		_framebuffer->Resize(event.Width, event.Height);
		_viewport.Resize(event.Width, event.Height);
		return false;
	};
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(TimeSpan ts)
{
	_camera.OnUpdate(ts);

	Renderer::SetTransform(_camera.View(), _camera.Projection());

	// Common
	_framebuffer->Bind();
	_topology.Bind();
	_viewport.Bind();
	_mesh->Bind();

	_framebuffer->Clear();
	Renderer::SubmitMesh(_mesh);
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


	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
