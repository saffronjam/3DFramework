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
		float x, y;
	};

	const VertexLayout layout({ElementType::Position2D});

	VertexStorage storage(layout);

	storage.Add(Vertex{0.0f, 0.5f});
	storage.Add(Vertex{0.5f, -0.5f});
	storage.Add(Vertex{-0.5f, -0.5f});

	_framebuffer = Framebuffer::Create({window.Width(), window.Height(), {ImageFormat::RGBA}});
	_vertexBuffer = VertexBuffer::Create(storage);
	_vertexShader = VertexShader::Create("VertexShader_v");
	_pixelShader = PixelShader::Create("PixelShader_p");
	_layout = InputLayout::Create(layout, _vertexShader);

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

void EditorLayer::OnUpdate(TimeSpan dt)
{
	_framebuffer->Bind();
	_vertexBuffer->Bind();
	_vertexShader->Bind();
	_pixelShader->Bind();
	_layout->Bind();
	_topology.Bind();
	_viewport.Bind();

	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			_framebuffer->Clear();
			package.Context.Draw(static_cast<UINT>(_vertexBuffer->VertexCount()), 0);
		}
	);
}

void EditorLayer::OnUi()
{
	_dockSpacePanel.Begin();

	_viewportPanel.OnUi();

	ImGui::ShowDemoWindow();

	_dockSpacePanel.End();
}
}
