#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneRenderer.h"

#include <ranges>

#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
SceneRenderer::SceneRenderer(Scene& scene) :
	_scene(scene),
	_transformCBuffer(TransformCBuffer::Create())
{
	// Construct empty containers for the duration of the object's lifetime
	_sceneCommon.DrawCommands.emplace(RenderChannel_Geometry, std::vector<DrawCommand>{});
	_sceneCommon.DrawCommands.emplace(RenderChannel_Shadow, std::vector<DrawCommand>{});
	_sceneCommon.DrawCommands.emplace(RenderChannel_Outline, std::vector<DrawCommand>{});

	_sceneCommon._sceneCommonCBuffer = ConstantBuffer<ShaderStructs::SceneCommonCBuffer>::Create(1);

	_renderGraph.RegisterOutput("BackBuffer", Renderer::BackBufferPtr());
	_renderGraph.RegisterInput("FinalOutput", _finalTarget);

	_renderGraph.Setup(_sceneCommon);

	_sceneCommon.LinesVertices.reserve(_sceneCommon.MaxLines);
	_sceneCommon.LineIndices.reserve(_sceneCommon.MaxLines);

	_sceneCommon.Environment = SceneEnvironment::Create(
		"Assets/Textures/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr"
	);

	SetViewportSize(100, 100);
}

void SceneRenderer::OnUi()
{
	_renderGraph.OnUi();
	_sceneCommon.Environment->OnUi();
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
			_sceneCommon.LinesVertices.clear();
			_sceneCommon.LineIndices.clear();
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

void SceneRenderer::SubmitModel(const std::shared_ptr<Model>& mesh, const Matrix& transform)
{
	for (const auto& activeContainer : _activeContainers)
	{
		activeContainer->second.emplace_back(DrawCommand{mesh, transform});
	}
}


void SceneRenderer::SubmitLine(const Vector3& from, const Vector3& to, const Color& color)
{
	if (_sceneCommon.LinesVertices.size() >= _sceneCommon.MaxLines)
	{
		throw SaffronException("Maximum number of lines has been reached");
	}

	const uint baseIndex = _sceneCommon.LinesVertices.size();

	_sceneCommon.LinesVertices.emplace_back(PosColVertex{from, color});
	_sceneCommon.LinesVertices.emplace_back(PosColVertex{to, color});

	_sceneCommon.LineIndices.emplace_back(baseIndex);
	_sceneCommon.LineIndices.emplace_back(baseIndex + 1);
}

void SceneRenderer::SubmitLines(const Vector3* positions, uint count, const Color& color)
{
	if (_sceneCommon.LinesVertices.size() + count > _sceneCommon.MaxLines)
	{
		throw SaffronException("Maximum number of lines has been reached");
	}

	Debug::Assert(count % 2 == 0, "Must be an even amount of positions");

	const uint baseIndex = _sceneCommon.LinesVertices.size();

	for (int i = 0; i < count; i += 2)
	{
		// Easier to debug if done in pairs
		_sceneCommon.LinesVertices.emplace_back(PosColVertex{positions[i], color});
		_sceneCommon.LinesVertices.emplace_back(PosColVertex{positions[i + 1], color});

		_sceneCommon.LineIndices.emplace_back(baseIndex + i);
		_sceneCommon.LineIndices.emplace_back(baseIndex + i);
	}
}

void SceneRenderer::SubmitLines(
	const Vector3* positions,
	uint positionCount,
	const uint* indices,
	uint indexCount,
	const Color& color
)
{
	if (_sceneCommon.LinesVertices.size() + positionCount > _sceneCommon.MaxLines)
	{
		throw SaffronException("Maximum number of lines has been reached");
	}

	Debug::Assert(indexCount % 2 == 0, "Must be an even amount of indices");

	const uint baseIndex = _sceneCommon.LinesVertices.size();

	for (int i = 0; i < positionCount; i += 2)
	{
		// Easier to debug if done in pairs
		_sceneCommon.LinesVertices.emplace_back(PosColVertex{positions[i], color});
		_sceneCommon.LinesVertices.emplace_back(PosColVertex{positions[i + 1], color});
	}
	for (int i = 0; i < indexCount; i += 2)
	{
		// Easier to debug if done in pairs
		_sceneCommon.LineIndices.emplace_back(baseIndex + indices[i]);
		_sceneCommon.LineIndices.emplace_back(baseIndex + indices[i + 1]);
	}
}

void SceneRenderer::SubmitCameraFrustum(const Camera& camera, const Matrix& view)
{
	DirectX::BoundingFrustum frustum;
	std::array<Vector3, 8> output2;
	frustum.GetCorners(output2.data());

	DirectX::BoundingFrustum::CreateFromMatrix(frustum, camera.Projection(), true);
	frustum.GetCorners(output2.data());
	frustum.Transform(frustum, view.Invert());

	std::array<Vector3, 8> positions;
	frustum.GetCorners(positions.data());

	const std::array<uint, 24> indices{
		// Far
		0, 1, 1, 2, 2, 3, 3, 0,

		// Near
		4, 5, 5, 6, 6, 7, 7, 4,

		// Between
		0, 4, 1, 5, 2, 6, 3, 7
	};

	SubmitLines(positions.data(), positions.size(), indices.data(), indices.size(), Colors::White);
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
