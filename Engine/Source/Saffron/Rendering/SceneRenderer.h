#pragma once

#include "Saffron/Rendering/Camera.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/RenderChannel.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Texture.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
struct SceneRendererOptions
{
	bool ShowGrid = true;
	bool ShowBoundingBoxes = false;
};

struct SceneRendererCamera
{
	Camera Camera;
	Matrix4f ViewMatrix;
	float Near, Far;
	float Fov;
};

struct DrawCommand
{
	Shared<Mesh> Mesh;
	Shared<MaterialInstance> Material;
	Matrix4f Transform;
};

struct SceneRendererCommon
{
	Shared<Shader> BloomBlurShader;
	Shared<Shader> BloomBlendShader;

	bool EnableBloom = false;
	float BloomThreshold = 1.5f;

	Vector2f FocusPoint = {0.5f, 0.5f};

	// Grid
	Shared<MaterialInstance> GridMaterial;
	Shared<MaterialInstance> OutlineMaterial;
};

struct SceneRendererStats
{
	float ShadowPass = 0.0f;
	float GeometryPass = 0.0f;
	float CompositePass = 0.0f;

	Timer ShadowPassTimer;
	Timer GeometryPassTimer;
	Timer CompositePassTimer;
};

struct SceneInfo
{
	const Scene* ActiveScene = nullptr;

	SceneRendererCamera SceneCamera;

	// Resources
	Shared<MaterialInstance> SkyboxMaterial;
	Shared<SceneEnvironment> SceneEnvironment;
	float SceneEnvironmentIntensity;
	LightEnvironment LightEnvironment;
	Light ActiveLight;

	void Clear()
	{
		ActiveScene = nullptr;
		SceneCamera = {};
		SkyboxMaterial = nullptr;
		SceneEnvironment = nullptr;
		LightEnvironment = {};
		ActiveLight = {};
	}
};

struct ShadowMapData
{
	float ShadowMapSize = 20.0f;
	float LightDistance = 0.1f;
	Matrix4f LightMatrices[4];
	Matrix4f LightViewMatrix;
	float CascadeSplitLambda = 0.91f;
	Vector4f CascadeSplits;
	float CascadeFarPlaneOffset = 15.0f, CascadeNearPlaneOffset = -15.0f;
	bool ShowCascades = false;
	bool SoftShadows = true;
	float LightSize = 0.5f;
	float MaxShadowDistance = 200.0f;
	float ShadowFade = 25.0f;
	float CascadeTransitionFade = 1.0f;
	bool CascadeFading = true;
};

class SceneRenderer : public SingleTon<SceneRenderer>
{
public:
	SceneRenderer();

	static void OnGuiRender();

	static void BeginScene(const class Scene* scene, const SceneRendererCamera& camera);
	static void EndScene();

	static void SubmitMesh(Shared<Mesh> mesh, const Matrix4f& transform = Matrix4f(1.0f),
	                       Shared<MaterialInstance> overrideMaterial = nullptr);
	static void SubmitSelectedMesh(Shared<Mesh> mesh, const Matrix4f& transform = Matrix4f(1.0f));

	static Shared<class SceneEnvironment> CreateEnvironmentMap(const Filepath& filepath);

	static const RenderPass& GetFinalRenderPass();
	static const Shared<Texture2D>& GetFinalColorBuffer();

	// TODO: Temp
	static uint32_t GetFinalColorBufferRendererID();
	static void SetFocusPoint(const Vector2f& point);

	static void SetViewportSize(Uint32 width, Uint32 height);

	static SceneRendererOptions& GetOptions();
	static ArrayList<DrawCommand>& GetDrawCommands(RenderChannel channel);
	static SceneRendererCommon& GetCommon();
	static SceneRendererStats& GetStats();
	static SceneInfo& GetSceneInfo();
	static ShadowMapData& GetShadowMapData();

private:
	static void FlushDrawList();
	static void ResetDrawContainer();

private:
	SceneRendererStats _stats;
	SceneRendererCommon _common;
	SceneRendererOptions _options;
	SceneInfo _sceneInfo;
	ShadowMapData _smData;

	Unique<RenderGraph> _renderGraph;
	UnorderedMap<RenderChannel, ArrayList<DrawCommand>> _drawContainer;
};
}
