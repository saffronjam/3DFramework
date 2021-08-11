#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/ShaderStructs.h"

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

struct SceneCommon
{
	CameraData CameraData;
	std::map<RenderChannels, std::vector<std::shared_ptr<Mesh>>> DrawCommands;
	PointLight PointLight;
};

class SceneRenderer
{
public:
	explicit SceneRenderer(Scene& scene);

	void OnUi();
	
	void Begin(const CameraData& cameraData);
	void End();

	void SubmitMesh(const std::shared_ptr<Mesh>& mesh, RenderChannels channels);

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

	RenderGraph _renderGraph;
	std::shared_ptr<Framebuffer> _finalTarget;
};
}
