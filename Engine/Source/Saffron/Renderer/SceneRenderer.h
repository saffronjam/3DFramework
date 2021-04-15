#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Scene/SceneEnvironment.h"

namespace Se
{
class Scene;

class SceneRenderer : public Instansiated<SceneRenderer>
{
private:
	struct FrustumBounds
	{
		float r, l, b, t, f, n;
	};

	struct CascadeData
	{
		Matrix4f ViewProj;
		Matrix4f View;
		float SplitDepth;
	};

public:
	struct Options
	{
		bool ShowGrid = true;
		bool ShowMeshBoundingBoxes = false;
		bool ShowPhysicsBodyBoundingBoxes = false;
	};

	struct CameraData
	{
		Shared<Camera> Camera;
		Matrix4f ViewMatrix{};
		float Near, Far;
		float FOV;
	};

	class Target : public MemManaged<Target>
	{
	private:
		struct RenderingStats
		{
			Time ShadowPass = 0.0f;
			Time GeometryPass = 0.0f;
			Time CompositePass = 0.0f;

			Timer ShadowPassTimer;
			Timer GeometryPassTimer;
			Timer CompositePassTimer;
		};

	public:
		explicit Target(String friendlyName);
		~Target();

		void Enable() { m_Enabled = true; }

		void Disable() { m_Enabled = false; }

		bool IsEnabled() const { return m_Enabled; }

		void GeometryPass();
		void CompositePass();
		void BloomBlurPass();
		void ShadowMapPass();

		const String& GetName() const { return m_FriendlyName; }

		const CameraData& GetCameraData() const { return m_CameraData; }

		void SetCameraData(const CameraData& cameraData) { m_CameraData = cameraData; };

		Vector2u GetSize();
		void SetSize(Uint32 width, Uint32 height);

		RenderingStats& GetRenderingStats() { return m_RenderingStats; }

		const RenderingStats& GetRenderingStats() const { return m_RenderingStats; }

		void ResetRenderingStats();

		const Shared<RenderPass>& GetGeoPass() const { return m_GeoPass; }

		const Shared<RenderPass>& GetCompositePass() const { return m_CompositePass; }

		const Shared<RenderPass>& GetBloomBlurPass(Uint32 index) const { return m_BloomBlurPass[index]; }

		const Shared<RenderPass>& GetBloomBlendPass() const { return m_BloomBlendPass; }

		const Shared<RenderPass>& GetShadowMapRenderPass(Uint32 index) const { return m_ShadowMapRenderPass[index]; }

		const Shared<Texture2D>& GetFinalColorBuffer() const; // TODO: Implement
		RendererID GetFinalColorBufferRendererID() const;

		void CalculateCascades(CascadeData* cascades, const Vector3f& lightDirection);

		static Shared<Target> Create(String friendlyName, Uint32 width, Uint32 height);

	private:
		String m_FriendlyName;
		RenderingStats m_RenderingStats;
		bool m_Enabled = true;
		CameraData m_CameraData;
		Shared<RenderPass> m_GeoPass;
		Shared<RenderPass> m_CompositePass;
		Shared<RenderPass> m_BloomBlurPass[2];
		Shared<RenderPass> m_BloomBlendPass;
		Shared<RenderPass> m_ShadowMapRenderPass[4];
	};

public:
	SceneRenderer();
	~SceneRenderer();

	static void BeginScene(const Scene* scene, ArrayList<Shared<Target>> targets);
	static void EndScene();

	static void OnGuiRender();

	static void SubmitMesh(const Shared<Mesh>& mesh, const Matrix4f& transform = Matrix4f(1.0f),
	                       const Shared<MaterialInstance>& overrideMaterial = nullptr);
	static void SubmitSelectedMesh(const Shared<Mesh>& mesh, const Matrix4f& transform = Matrix4f(1.0f));
	static void SubmitLine(const Vector3f& first, const Vector3f& second, const Vector4f& color);
	static void SubmitAABB(const AABB& aabb, const Matrix4f& transform, const Vector4f& color = Vector4f(1.0f));
	static void SubmitAABB(Shared<Mesh> mesh, const Matrix4f& transform, const Vector4f& color = Vector4f(1.0f));

	static Shared<SceneEnvironment> CreateSceneEnvironment(const Filepath &filepath);

	static Shared<Target>& SceneRenderer::GetMainTarget();
	static Options& GetOptions();

	static void SetFocusPoint(const Vector2f& focusPoint);

private:
	static void FlushDrawList();

private:
	Unique<struct SceneRendererData> _data;
};
}
