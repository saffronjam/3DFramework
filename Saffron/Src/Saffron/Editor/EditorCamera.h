#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/System/Time.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Core/Event/MouseEvent.h"
#include "Saffron/Core/Event/WindowEvent.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class EditorCamera : public Camera
{
public:
	EditorCamera() = default;
	explicit EditorCamera(const glm::mat4 &projectionMatrix);

	void Focus();
	void OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts);
	void OnEvent(const Event &event);

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

	void OnMouseScroll(const MouseScrollEvent &event);
	void OnMousePan(const glm::vec2 &delta);
	void OnMouseRotate(const glm::vec2 &delta);
	void OnMouseZoom(float delta);

	glm::vec3 CalculatePosition();

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
