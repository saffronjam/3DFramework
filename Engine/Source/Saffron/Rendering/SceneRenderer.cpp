#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneRenderer.h"

#include <ranges>

#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Graphics/Scene.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
SceneRenderer::SceneRenderer(Scene& scene) :
	_scene(scene),
	_mvpCBuffer(MvpCBuffer::Create())
{
	// Construct empty containers for the duration of the object's lifetime
	_sceneCommon.DrawCommands.emplace(RenderChannel_Geometry, std::vector<DrawCommand>{});
	_sceneCommon.DrawCommands.emplace(RenderChannel_Shadow, std::vector<DrawCommand>{});
	_sceneCommon.DrawCommands.emplace(RenderChannel_Outline, std::vector<DrawCommand>{});

	_renderGraph.RegisterOutput("BackBuffer", Renderer::BackBufferPtr());
	_renderGraph.RegisterInput("FinalOutput", _finalTarget);

	_renderGraph.Setup(_sceneCommon);

	_sceneCommon.Lines.reserve(_sceneCommon.MaxLines);

	SetViewportSize(100, 100);
}

void SceneRenderer::OnUi()
{
	_renderGraph.OnUi();
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
			_sceneCommon.Lines.clear();
		}
	);
}

void SceneRenderer::BeginSubmtions(RenderChannels channels)
{
	const auto addContainer = [this](uint channel)
	{
		_activeContainers.emplace_back(
			std::ranges::find_if(
				_sceneCommon.DrawCommands,
				[channel](const auto& pair)
				{
					return pair.first == channel;
				}
			)
		);
	};

	if (channels & RenderChannel_Geometry)
	{
		addContainer(RenderChannel_Geometry);
	}
	if (channels & RenderChannel_Shadow)
	{
		addContainer(RenderChannel_Shadow);
	}
	if (channels & RenderChannel_Outline)
	{
		addContainer(RenderChannel_Outline);
	}
}

void SceneRenderer::EndSubmtions()
{
	_activeContainers.clear();
}

void SceneRenderer::SubmitMesh(const std::shared_ptr<Mesh>& mesh, const Matrix& transform)
{
	for (const auto& activeContainer : _activeContainers)
	{
		activeContainer->second.emplace_back(DrawCommand{mesh, transform});
	}
}


void SceneRenderer::SubmitLine(const Vector3& from, const Vector3& to, const Color& color)
{
	if (_sceneCommon.Lines.size() >= _sceneCommon.MaxLines)
	{
		throw SaffronException("Maximum number of lines has been reached");
	}

	_sceneCommon.Lines.emplace_back(PosColVertex{from, color}, PosColVertex{to, color});
}

void SceneRenderer::SubmitLines(const Vector3* positions, uint count, const Color& color)
{
	if (_sceneCommon.Lines.size() + count > _sceneCommon.MaxLines)
	{
		throw SaffronException("Maximum number of lines has been reached");
	}

	Debug::Assert(count % 2 == 0, "Must be an even amount of positions");

	for (int i = 0; i < count; i += 2)
	{
		_sceneCommon.Lines.emplace_back(PosColVertex{positions[i], color}, PosColVertex{positions[i + 1], color});
	}
}

void SceneRenderer::SubmitRect(const FloatRect& rect, const Shader& shader)
{
}

void SceneRenderer::SubmitFullscreenRect(const Shader& shader)
{
	SubmitRect({-1.0f, -1.0f, 2.0f, 2.0f}, shader);
}

void SceneRenderer::SubmitCameraFrustum(const Camera& camera, const Matrix& view)
{
	DirectX::BoundingFrustum frustum;
	std::array<Vector3, 8> output2;
	frustum.GetCorners(output2.data());

	DirectX::BoundingFrustum::CreateFromMatrix(frustum, camera.Projection(), true);
	frustum.GetCorners(output2.data());
	frustum.Transform(frustum, view.Invert());

	std::array<Vector3, 8> output;
	frustum.GetCorners(output.data());

	SubmitLine(output[0], output[1], Colors::White);
	SubmitLine(output[1], output[2], Colors::White);
	SubmitLine(output[2], output[3], Colors::White);
	SubmitLine(output[3], output[0], Colors::White);

	SubmitLine(output[4], output[5], Colors::White);
	SubmitLine(output[5], output[6], Colors::White);
	SubmitLine(output[6], output[7], Colors::White);
	SubmitLine(output[7], output[4], Colors::White);

	SubmitLine(output[0], output[4], Colors::White);
	SubmitLine(output[1], output[5], Colors::White);
	SubmitLine(output[3], output[7], Colors::White);
	SubmitLine(output[2], output[6], Colors::White);
}

void SceneRenderer::SetViewportSize(uint width, uint height)
{
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

auto SceneRenderer::SceneCommon() -> Se::SceneCommon&
{
	return _sceneCommon;
}

auto SceneRenderer::SceneCommon() const -> const Se::SceneCommon&
{
	return _sceneCommon;
}
}
