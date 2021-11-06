#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/ShaderStructs.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/SceneEnvironment.h"
#include "Saffron/Rendering/VertexTypes.h"

namespace Se
{
struct Mvp;
class TransformCBuffer;
class Model;
class Scene;
struct CameraData;

using RenderChannels = uint;

enum RenderChannel_ : uint
{
	RenderChannel_Geometry = BIT(0),
	RenderChannel_Shadow = BIT(1),
	RenderChannel_Outline = BIT(2),
};

struct DrawCommand
{
	std::shared_ptr<Model> Model;
	Matrix Transform;
};

namespace ShaderStructs
{
struct alignas(16) SceneCommonCBuffer
{
	Vector3 CameraPosition;
};
}

struct SceneCommon
{
	CameraData CameraData;
	std::shared_ptr<ConstantBuffer<ShaderStructs::SceneCommonCBuffer>> _sceneCommonCBuffer;

	std::map<RenderChannels, std::vector<DrawCommand>> DrawCommands{};

	// Shadow and light
	ShaderStructs::PointLight PointLight;

	// Lines
	static constexpr uint MaxLines = 10000;
	std::vector<PosColVertex> LinesVertices{};
	std::vector<uint> LineIndices{};

	// Quads
	static constexpr uint MaxQuads = 1000;
	std::vector<PosColTexVertex> QuadVertices{};
	std::vector<uint> QuadIndices{};

	// Environment and skybox
	std::shared_ptr<SceneEnvironment> Environment;
};

class SceneRenderer
{
public:
	explicit SceneRenderer(Scene& scene);

	void OnUi();

	void Begin(const CameraData& cameraData);
	void End();

	void BeginSubmtions(RenderChannels channels);
	void EndSubmtions();

	void SubmitModel(const std::shared_ptr<Model>& mesh, const Matrix& transform = Matrix::Identity);
	void SubmitLine(const Vector3& from, const Vector3& to, const Color& color);
	void SubmitLines(const Vector3* positions, uint count, const Color& color);
	void SubmitLines(
		const Vector3* positions,
		uint positionCount,
		const uint* indices,
		uint indexCount,
		const Color& color
	);
	void SubmitCameraFrustum(const Camera& camera, const Matrix& view);

	void SetViewportSize(uint width, uint height);

	auto FinalTarget() const -> const Framebuffer&;
	auto FinalTargetPtr() const -> const std::shared_ptr<Framebuffer>&;

	auto SceneCommon() -> struct SceneCommon&;
	auto SceneCommon() const -> const struct SceneCommon&;

public:
	mutable SubscriberList<const SizeEvent&> ViewportResized;

private:
	Scene& _scene;
	struct SceneCommon _sceneCommon;
	std::shared_ptr<TransformCBuffer> _transformCBuffer;
	std::vector<std::map<RenderChannels, std::vector<DrawCommand>>::iterator> _activeContainers;

	RenderGraph _renderGraph;
	std::shared_ptr<Framebuffer> _finalTarget;
};
}
