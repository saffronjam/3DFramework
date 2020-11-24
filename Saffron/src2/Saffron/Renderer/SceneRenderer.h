#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class Scene;
class SceneRenderer
{
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
	};

	class Target : public ReferenceCounted
	{
	public:
		Target() = default;
		~Target();

		void Enable() { m_Enabled = true; }
		void Disable() { m_Enabled = false; }
		bool IsEnabled() const { return m_Enabled; }

		const CameraData &GetCameraData() const { return m_CameraData; }
		void SetCameraData(const CameraData &cameraData) { m_CameraData = cameraData; };

		Vector2u GetSize();
		void SetSize(Uint32 width, Uint32 height);

		const Shared<RenderPass> &GetGeoPass()const { return m_GeoPass; }
		const Shared<RenderPass> &GetCompositePass() const { return m_CompositePass; }
		const Shared<Texture2D> &GetFinalColorBuffer() const; // TODO: Implement
		RendererID GetFinalColorBufferRendererID() const;

		static Shared<Target> Create(Uint32 width, Uint32 height);

	private:
		bool m_Enabled = true;
		CameraData m_CameraData;
		Shared<RenderPass> m_GeoPass;
		Shared<RenderPass> m_CompositePass;
	};

public:
	static void Init();

	static void BeginScene(const Scene *scene, ArrayList<Shared<Target>> targets);
	static void EndScene();

	static void SubmitMesh(const Shared<Mesh> &mesh, const Matrix4f &transform = Matrix4f(1.0f), const Shared<MaterialInstance>
						   &overrideMaterial = nullptr);
	static void SubmitSelectedMesh(const Shared<Mesh> &mesh, const Matrix4f &transform = Matrix4f(1.0f));
	static void SubmitLine(const Vector3f &first, const Vector3f &second, const Vector4f &color);
	static void SubmitAABB(const AABB &aabb, const Matrix4f &transform, const Vector4f &color = Vector4f(1.0f));
	static void SubmitAABB(Shared<Mesh> mesh, const Matrix4f &transform, const Vector4f &color = Vector4f(1.0f));

	static Pair<Shared<TextureCube>, Shared<TextureCube>> CreateEnvironmentMap(const String &filepath);

	static Shared<Target> &GetMainTarget();
	static Options &GetOptions();
private:
	static void FlushDrawList();
	static void GeometryPass(const Shared<Target> &target);
	static void CompositePass(const Shared<Target> &target);
};

}

