#pragma once

#include "Saffron/Core/Events/MouseEvent.h"
#include "Saffron/Core/Events/KeyboardEvent.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Core/Time.h"
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
	EditorCamera();
	explicit EditorCamera(Matrix4f projectionMatrix);

	void OnUpdate();
	void OnGuiRender();

	void Reset();

	void Enable() { m_Enabled = true; }

	void Disable() { m_Enabled = false; }

	bool IsEnabled() const { return m_Enabled; }

	const Matrix4f& GetViewMatrix() const { return m_ViewMatrix; }

	Matrix4f GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

	const Vector3f& GetPosition() const { return m_Position; }

	float GetPitch() const { return m_Pitch; }

	float GetYaw() const { return m_Yaw; }

	float GetRoll() const { return 0.0f; }

	void SetPosition(const Vector3f& position);
	void SetRotation(const Vector3f& rotation);

	Vector3f GetUpDirection() const;
	Vector3f GetRightDirection() const;
	Vector3f GetForwardDirection() const;

	ControllerStyle GetControllerStyle() const { return m_ControllerStyle; }

	void SetControllerStyle(ControllerStyle style) { m_ControllerStyle = style; }

private:
	bool OnMouseScroll(const MouseWheelScrolledEvent& event);

	void UpdateCameraView();

	float GetZoomSpeed() const;

private:
	bool m_Enabled = true;
	float m_MovementSpeed = 10.0f;
	ControllerStyle m_ControllerStyle = ControllerStyle::Game;

	Matrix4f m_ViewMatrix{};
	Vector3f m_Position{0.0f, 0.0f, 0.0f};
	Array<bool, 6> m_Keystates{false, false, false, false, false, false};

	Vector3f m_Forward{0.0f, 0.0f, 0.0f};
	Vector3f m_Up{0.0f, 0.0f, 0.0f};
	Vector3f m_Right{0.0f, 0.0f, 0.0f};
	Vector3f m_WorldUp{0.0f, 1.0f, 0.0f};

	float m_Pitch = 0.0f, m_Yaw = 0.0f;
};
}
