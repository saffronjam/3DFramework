#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Camera.h"

namespace Se
{
class SceneCamera : public Camera
{
public:
	enum class ProjectionMode : int { Perspective = 0, Orthographic = 1 };

public:
	SceneCamera(uint width = 1280, uint height = 720, ProjectionMode projectionType = ProjectionMode::Perspective);

	void RenderFrustum(const Matrix4& transform) const;

	void SetPerspective(float verticalFOV, float nearClip = 0.1f, float farClip = 2000.0f);
	void SetOrthographic(float size, float nearClip = -1.0f, float farClip = 1.0f);

	Pair<uint, uint> GetViewportSize() const { return {_viewportWidth, _viewportHeight}; }

	uint GetViewportWidth() const { return _viewportWidth; }

	uint GetViewportHeith() const { return _viewportHeight; }

	void SetViewportSize(uint width, uint height) override;

	void SetPerspectiveVerticalFOV(float verticalFov) { _perspectiveFOV = glm::radians(verticalFov); }

	float GetPerspectiveVerticalFOV() const { return glm::degrees(_perspectiveFOV); }

	void SetPerspectiveNearClip(float nearClip) { _perspectiveNear = nearClip; }

	float GetPerspectiveNearClip() const { return _perspectiveNear; }

	void SetPerspectiveFarClip(float farClip) { _perspectiveFar = farClip; }

	float GetPerspectiveFarClip() const { return _perspectiveFar; }

	void SetOrthographicSize(float size) { _orthographicSize = size; }

	float GetOrthographicSize() const { return _orthographicSize; }

	void SetOrthographicNearClip(float nearClip) { _orthographicNear = nearClip; }

	float GetOrthographicNearClip() const { return _orthographicNear; }

	void SetOrthographicFarClip(float farClip) { _orthographicFar = farClip; }

	float GetOrthographicFarClip() const { return _orthographicFar; }

	void SetProjectionMode(ProjectionMode type) { _projectionMode = type; }

	ProjectionMode GetProjectionMode() const { return _projectionMode; }

private:
	ProjectionMode _projectionMode = ProjectionMode::Perspective;

	float _perspectiveFOV = glm::radians(45.0f);
	float _perspectiveNear = 0.1f, _perspectiveFar = 2000.0f;

	float _orthographicSize = 10.0f;
	float _orthographicNear = -1.0f, _orthographicFar = 1.0f;
};
}
