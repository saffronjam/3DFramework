#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Camera.h"

namespace Se
{
class SceneCamera : public Camera
{
public:
	enum class ProjectionMode : Int32 { Perspective = 0, Orthographic = 1 };

public:
	SceneCamera(Uint32 width = 1280, Uint32 height = 720, ProjectionMode projectionType = ProjectionMode::Perspective);

	void RenderFrustum(const Matrix4f& transform) const;

	void SetPerspective(float verticalFOV, float nearClip = 0.1f, float farClip = 2000.0f);
	void SetOrthographic(float size, float nearClip = -1.0f, float farClip = 1.0f);

	Pair<Uint32, Uint32> GetViewportSize() const { return {m_ViewportWidth, m_ViewportHeight}; }

	Uint32 GetViewportWidth() const { return m_ViewportWidth; }

	Uint32 GetViewportHeith() const { return m_ViewportHeight; }

	void SetViewportSize(Uint32 width, Uint32 height) override;

	void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = glm::radians(verticalFov); }

	float GetPerspectiveVerticalFOV() const { return glm::degrees(m_PerspectiveFOV); }

	void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; }

	float GetPerspectiveNearClip() const { return m_PerspectiveNear; }

	void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; }

	float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

	void SetOrthographicSize(float size) { m_OrthographicSize = size; }

	float GetOrthographicSize() const { return m_OrthographicSize; }

	void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; }

	float GetOrthographicNearClip() const { return m_OrthographicNear; }

	void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; }

	float GetOrthographicFarClip() const { return m_OrthographicFar; }

	void SetProjectionMode(ProjectionMode type) { m_ProjectionMode = type; }

	ProjectionMode GetProjectionMode() const { return m_ProjectionMode; }

private:
	ProjectionMode m_ProjectionMode = ProjectionMode::Perspective;

	float m_PerspectiveFOV = glm::radians(45.0f);
	float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 2000.0f;

	float m_OrthographicSize = 10.0f;
	float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;
};
}
