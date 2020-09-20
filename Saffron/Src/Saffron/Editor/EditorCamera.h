#pragma once

#include "Saffron/Core/Events/MouseEvent.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/Camera.h"

namespace Se
{
class EditorCamera : public Camera
{
public:
	EditorCamera() = default;
	explicit EditorCamera(const glm::mat4 &projectionMatrix);

	void Focus();
	void OnUpdate(Time ts);
	bool OnEvent(const Event &event);

	float GetDistance() const { return m_Distance; }

	const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
	glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;
	const glm::vec3 &GetPosition() const { return m_Position; }
	glm::quat GetOrientation() const;

	float GetExposure() const override { return m_Exposure; }
	float &GetExposure() override { return m_Exposure; }

	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }

	void SetDistance(float distance) { m_Distance = distance; }
	void SetViewportSize(Uint32 width, Uint32 height) { m_ViewportWidth = width; m_ViewportHeight = height; }

private:
	void UpdateCameraView();

	bool OnMouseScroll(const MouseScrollEvent &event);

	void MousePan(const glm::vec2 &delta);
	void MouseRotate(const glm::vec2 &delta);
	void MouseZoom(float delta);

	glm::vec3 CalculatePosition() const;

	glm::vec2 GetPanSpeed() const;
	float GetRotationSpeed() const;
	float GetZoomSpeed() const;

private:
	glm::mat4 m_ViewMatrix{};
	glm::vec3 m_Position{}, m_Rotation{}, m_FocalPoint{};

	bool m_Panning{}, m_Rotating{};
	glm::vec2 m_InitialMousePosition{};
	glm::vec3 m_InitialFocalPoint{}, m_InitialRotation{};

	float m_Distance;
	float m_Pitch, m_Yaw;

	float m_Exposure = 0.8f;

	Uint32 m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}
