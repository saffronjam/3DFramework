#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/ShaderStructs.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Graphics/Shapes.h"

namespace Se
{
struct Mvp;
class MvpCBuffer;
class Mesh;
class Scene;
struct CameraData;

using RenderChannels = uint;

enum RenderChannel_ : uint
{
	RenderChannel_Geometry = BIT(0),
	RenderChannel_Shadow = BIT(1),
	RenderChannel_Outline = BIT(2),
};

struct PosColVertex
{
	Vector3 Position;
	Color Color;

	static auto Layout() -> VertexLayout
	{
		return {{"Position", ElementType::Float3}, {"Color", ElementType::Float4}};
	}
};

struct PosTexColVertex
{
	Vector3 Position;
	Vector2 TexCoords;
	Color Color;

	static auto Layout() -> VertexLayout
	{
		return {{"Position", ElementType::Float3}, {"TexCoord", ElementType::Float2}, {"Color", ElementType::Float4}};
	}
};

struct DrawCommand
{
	std::shared_ptr<Mesh> Mesh;
	Matrix Transform;
};

struct SceneCommon
{
	CameraData CameraData;
	std::map<RenderChannels, std::vector<DrawCommand>> DrawCommands;

	// Shadow and light
	PointLight PointLight;

	// Lines
	static constexpr uint MaxLines = 10000;
	std::vector<std::pair<PosColVertex, PosColVertex>> Lines;
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

	void SubmitMesh(const std::shared_ptr<Mesh>& mesh, const Matrix& transform = Matrix::Identity);
	void SubmitLine(const Vector3& from, const Vector3& to, const Color& color);
	void SubmitLines(const Vector3* positions, uint count, const Color& color);
	void SubmitRect(const FloatRect& rect, const Shader& shader);
	void SubmitFullscreenRect(const Shader& shader);
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
	std::shared_ptr<MvpCBuffer> _mvpCBuffer;
	std::vector<std::map<RenderChannels, std::vector<DrawCommand>>::iterator> _activeContainers;

	RenderGraph _renderGraph;
	std::shared_ptr<Framebuffer> _finalTarget;
};
}
