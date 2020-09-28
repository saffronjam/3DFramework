#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
struct SceneRendererCameraData
{
	Camera Camera;
	glm::mat4 ViewMatrix;
};

class SceneRenderer
{
public:
	struct Options
	{
		bool ShowGrid = true;
		bool ShowBoundingBoxes = false;
	};


public:
	static void Init();

	static void SetViewportSize(Uint32 width, Uint32 height);

	static void BeginScene(const Scene *scene, const SceneRendererCameraData &camera);
	static void EndScene();

	static void SubmitMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform = glm::mat4(1.0f), const Ref<MaterialInstance>
						   &overrideMaterial = nullptr);
	static void SubmitSelectedMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform = glm::mat4(1.0f));

	static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string &filepath);

	static Ref<RenderPass> GetFinalRenderPass();
	static Ref<Texture2D> GetFinalColorBuffer();
	// TODO: Temp
	static Uint32 GetFinalColorBufferRendererID();
	static Options &GetOptions();
private:
	static void FlushDrawList();
	static void GeometryPass();
	static void CompositePass();
};

}

