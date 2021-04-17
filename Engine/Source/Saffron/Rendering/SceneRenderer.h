#pragma once

#include "Saffron/Rendering/Camera.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Texture.h"

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
	glm::mat4 ViewMatrix;
	float Near, Far;
	float FOV;
};

class SceneRenderer : public SingleTon<SceneRenderer>
{
public:
	SceneRenderer();
	~SceneRenderer();

	static void OnGuiRender();

	static void BeginScene(const class Scene* scene, const SceneRendererCamera& camera);
	static void EndScene();

	static void SubmitMesh(Shared<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f),
	                       Shared<MaterialInstance> overrideMaterial = nullptr);
	static void SubmitSelectedMesh(Shared<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));

	static Shared<class SceneEnvironment> CreateEnvironmentMap(const Filepath& filepath);

	static Shared<RenderPass> GetFinalRenderPass();
	static Shared<Texture2D> GetFinalColorBuffer();

	// TODO: Temp
	static uint32_t GetFinalColorBufferRendererID();
	static void SetFocusPoint(const glm::vec2& point);

	static void SetViewportSize(uint32_t width, uint32_t height);

	static SceneRendererOptions& GetOptions();

private:
	static void FlushDrawList();
	static void GeometryPass();
	static void CompositePass();
	static void BloomBlurPass();
	static void ShadowMapPass();

	static void CalculateCascades(struct CascadeData* cascades, const glm::vec3& lightDirection);

private:
	struct SceneRendererStats* _stats;
	struct SceneRendererData* _data;
};
}
