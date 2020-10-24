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
	Matrix4f ViewMatrix{};
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

	static void AddRenderTarget(const String &renderTargetIdentifier, Uint32 width, Uint32 height);
	static void SetRenderTargetSize(const String &renderTargetIdentifier, Uint32 width, Uint32 height);
	static void SetCameraData(const String &renderTargetIdentifier, const SceneRendererCameraData &cameraData);

	static void EnableRenderTarget(const String &renderTargetIdentifier);
	static void DisableRenderTarget(const String &renderTargetIdentifier);
	static bool IsRenderTargetEnabled(const String &renderTargetIdentifier);

	static void BeginScene(const Scene *scene);
	static void EndScene();

	static void SubmitMesh(const Shared<Mesh> &mesh, const Matrix4f &transform = Matrix4f(1.0f), const Shared<MaterialInstance>
						   &overrideMaterial = nullptr);
	static void SubmitSelectedMesh(const Shared<Mesh> &mesh, const Matrix4f &transform = Matrix4f(1.0f));

	static std::pair<Shared<TextureCube>, Shared<TextureCube>> CreateEnvironmentMap(const String &filepath);

	static Shared<RenderPass> GetFinalRenderPass(const String &renderTargetIdentifier);
	static Shared<Texture2D> GetFinalColorBuffer(const String &renderTargetIdentifier);
	// TODO: Temp
	static Uint32 GetFinalColorBufferRendererID(const String &renderTargetIdentifier);
	static Options &GetOptions();
private:
	static void FlushDrawList();
	static void GeometryPass(const String &renderTargetIdentifier);
	static void CompositePass(const String &renderTargetIdentifier);
};

}

