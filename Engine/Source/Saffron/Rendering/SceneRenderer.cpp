#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneRenderer.h"

#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
SceneRenderer::SceneRenderer(Scene& scene) :
	_scene(scene),
	_mvpCBuffer(MvpCBuffer::Create())
{
	// Construct empty containers for the duration of the object's lifetime
	_sceneCommon.DrawCommands.emplace(RenderChannel_Geometry, std::vector<std::shared_ptr<Mesh>>{});
	_sceneCommon.DrawCommands.emplace(RenderChannel_Shadow, std::vector<std::shared_ptr<Mesh>>{});
	_sceneCommon.DrawCommands.emplace(RenderChannel_Outline, std::vector<std::shared_ptr<Mesh>>{});

	_renderGraph.RegisterOutput("backBuffer", Renderer::BackBufferPtr());
	_renderGraph.RegisterInput("finalOutput", _finalTarget);

	_renderGraph.Setup(_sceneCommon);

	SetViewportSize(100, 100);
}

void SceneRenderer::Begin(const CameraData& cameraData)
{
	_sceneCommon.CameraData = cameraData;
}

void SceneRenderer::End()
{
	_renderGraph.Execute();

	Renderer::Submit(
		[this](const RendererPackage&)
		{
			for (auto& channelCont : _sceneCommon.DrawCommands | std::views::values)
			{
				channelCont.clear();
			}
		}
	);
}

void SceneRenderer::SubmitMesh(const std::shared_ptr<Mesh>& mesh, RenderChannels channels)
{
	if (channels & RenderChannel_Geometry)
	{
		_sceneCommon.DrawCommands.at(RenderChannel_Geometry).emplace_back(mesh);
	}
	if (channels & RenderChannel_Shadow)
	{
		_sceneCommon.DrawCommands.at(RenderChannel_Shadow).emplace_back(mesh);
	}
	if (channels & RenderChannel_Outline)
	{
		_sceneCommon.DrawCommands.at(RenderChannel_Outline).emplace_back(mesh);
	}
}

void SceneRenderer::SetViewportSize(uint width, uint height)
{
	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			D3D11_VIEWPORT viewport{0.0f, 0.0f, width, height, 0.0f, 1.0f};
			package.Context.RSSetViewports(1, &viewport);
		}
	);
	_renderGraph.SetViewportSize(width, height);

	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			ViewportResized.Invoke({width, height});
		}
	);
}

auto SceneRenderer::FinalTarget() const -> const Framebuffer&
{
	return *_finalTarget;
}

auto SceneRenderer::FinalTargetPtr() const -> const std::shared_ptr<Framebuffer>&
{
	return _finalTarget;
}
}
