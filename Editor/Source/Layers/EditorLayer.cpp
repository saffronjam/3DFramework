#include "Layers/EditorLayer.h"

namespace Se
{
EditorLayer::EditorLayer() :
	_viewportPanel("Viewport"),
	_framebuffer(Framebuffer::Create(FramebufferSpec{500, 500, {ImageFormat::RGBA}})),
	_viewport(App::Instance().Window().Width(), App::Instance().Window().Height())
{
}

void EditorLayer::OnAttach()
{
	const auto& window = App::Instance().Window();

	struct Vertex
	{
		float x, y, z;
	};
	
	_mesh = Mesh::Create("Torus.fbx");	
	_mvpCBuffer = MvpCBuffer::Create({Matrix::Identity, _camera.View(), _camera.Projection()});

	_framebuffer->Resized += [this](const SizeEvent& event)
	{
		_viewportPanel.SetImage(_framebuffer->FinalTargetPtr());
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
	_mvpCBuffer->UpdateMatrix({Matrix::Identity, _camera.View(), _camera.Projection()});
	
	// Common
	_framebuffer->Bind();
	_topology.Bind();
	_viewport.Bind();
	_mvpCBuffer->Bind();
	
	_mesh->Bind();	
	
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;

			ComPtr<ID3D11RasterizerState> _nativeRasterizer;
			package.Device.CreateRasterizerState(&rd, &_nativeRasterizer);

			package.Context.RSSetState(_nativeRasterizer.Get());
		}
	);
	
	_framebuffer->Clear();
	Renderer::SubmitMesh(_mesh);
}

void EditorLayer::OnUi()
{
	_dockSpacePanel.Begin();

	_viewportPanel.OnUi();

	_camera.OnUi();

	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
