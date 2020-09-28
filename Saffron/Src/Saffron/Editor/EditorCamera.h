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
	enum class ControllerStyle
	{
		Game = 0,
		Maya = 1
	};

public:
	EditorCamera() = default;
	explicit EditorCamera(const glm::mat4 &projectionMatrix);

	void OnUpdate(Time ts);
	bool OnEvent(const Event &event);

	void Reset();

	ControllerStyle GetControllerStyle()const { return m_ControllerStyle; }
	const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
	glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }
	float GetDistance() const { return m_Distance; }
	const glm::vec3 &GetPosition() const { return m_Position; }
	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }
	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;
	glm::quat GetOrientation() const;

	void SetControllerStyle(ControllerStyle style) { m_ControllerStyle = style; }
	void SetDistance(float distance) { m_Distance = distance; }

private:
	bool OnMouseScroll(const MouseScrollEvent &event);

	void UpdateCameraView();
	glm::vec3 CalculatePosition() const;

	void MousePan(const glm::vec2 &delta);
	void MouseRotate(const glm::vec2 &delta);
	void MouseZoom(float delta);

	glm::vec2 GetPanSpeed() const;
	float GetRotationSpeed() const;
	float GetZoomSpeed() const;

private:
	float m_MovementSpeed = 10.0f;
	ControllerStyle	m_ControllerStyle = ControllerStyle::Maya;

	glm::mat4 m_ViewMatrix{};
	glm::vec3 m_Position{}, m_Rotation{}, m_FocalPoint{};

	float m_Distance{};
	float m_Pitch{}, m_Yaw{};


};
}
