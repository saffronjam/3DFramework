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
	const glm::vec3 &GetPosition() const { return m_Position; }
	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }
	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;

	void SetControllerStyle(ControllerStyle style) { m_ControllerStyle = style; }

private:
	bool OnMouseScroll(const MouseScrollEvent &event);

	void UpdateCameraView();


	float GetZoomSpeed() const;

private:
	float m_MovementSpeed = 10.0f;
	ControllerStyle	m_ControllerStyle = ControllerStyle::Game;

	glm::mat4 m_ViewMatrix{};
	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

	glm::vec3 m_Forward{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Up{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Right{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_WorldUp{ 0.0f, 1.0f, 0.0f };

	float m_Pitch = 0.0f, m_Yaw = 0.0f;


};
}
