#pragma once

#include "Saffron/Common/Events/MouseEvent.h"
#include "Saffron/Common/Events/WindowEvent.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/Camera.h"

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
	explicit EditorCamera(Matrix projectionMatrix);

	void OnUpdate();
	void OnGuiRender();
	bool OnEvent(const Event &event);

	void Reset();
	void Enable() { _enabled = true; }
	void Disable() { _enabled = false; }
	bool IsEnabled() const { return _enabled; }

	const Matrix &GetViewMatrix() const { return _viewMatrix; }
	Matrix GetViewProjection() const { return _projectionMatrix * _viewMatrix; }

	const Vector3 &GetPosition() const { return _position; }
	float GetPitch() const { return _pitch; }
	float GetYaw() const { return m_Yaw; }
	float GetRoll() const { return 0.0f; }

	void SetPosition(const Vector3 &position);
	void SetRotation(const Vector3 &rotation);

	Vector3 GetUpDirection() const;
	Vector3 GetLeftDirection() const;
	Vector3 GetForwardDirection() const;

	ControllerStyle GetControllerStyle()const { return _controllerStyle; }
	void SetControllerStyle(ControllerStyle style) { _controllerStyle = style; }

private:
	bool OnMouseScroll(const MouseWheelScrolledEvent &event);

	void UpdateCameraView();


	float GetZoomSpeed() const;

private:
	bool _enabled = true;
	float _movementSpeed = 10.0f;
	ControllerStyle	_controllerStyle = ControllerStyle::Game;

	Matrix _viewMatrix{};
	Vector3 _position{ 0.0f, 0.0f, 0.0f };

	Vector3 _forward{ 0.0f, 0.0f, 0.0f };
	Vector3 _up{ 0.0f, 0.0f, 0.0f };
	Vector3 _left{ 0.0f, 0.0f, 0.0f };
	Vector3 _worldUp{ 0.0f, 1.0f, 0.0f };

	float _pitch = 0.0f, m_Yaw = 0.0f;

};
}
