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
	const Camera *Camera = nullptr;
	glm::mat4 ViewMatrix{};
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

	static void AddRenderTarget(const std::string &renderTargetIdentifier, Uint32 width, Uint32 height);
	static void SetRenderTargetSize(const std::string &renderTargetIdentifier, Uint32 width, Uint32 height);
	static void SetCameraData(const std::string &renderTargetIdentifier, const SceneRendererCameraData &cameraData);

	static void EnableRenderTarget(const std::string &renderTargetIdentifier);
	static void DisableRenderTarget(const std::string &renderTargetIdentifier);
	static bool IsRenderTargetEnabled(const std::string &renderTargetIdentifier);

	static void BeginScene(const Scene *scene);
	static void EndScene();

	static void SubmitMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform = glm::mat4(1.0f), const Ref<MaterialInstance>
						   &overrideMaterial = nullptr);
	static void SubmitSelectedMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform = glm::mat4(1.0f));

	static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string &filepath);

	static Ref<RenderPass> GetFinalRenderPass(const std::string &renderTargetIdentifier);
	static Ref<Texture2D> GetFinalColorBuffer(const std::string &renderTargetIdentifier);
	// TODO: Temp
	static Uint32 GetFinalColorBufferRendererID(const std::string &renderTargetIdentifier);
	static Options &GetOptions();
private:
	static void FlushDrawList();
	static void GeometryPass(const std::string &renderTargetIdentifier);
	static void CompositePass(const std::string &renderTargetIdentifier);
};

}

