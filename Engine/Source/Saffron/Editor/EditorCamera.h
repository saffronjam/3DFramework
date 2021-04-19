#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/MouseEvent.h"
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
	explicit EditorCamera(Matrix4f projectionMatrix);

	void OnUpdate();
	void OnGuiRender();

	void Reset();

	void Enable();
	void Disable();
	bool IsEnabled() const;

	const Matrix4f& GetViewMatrix() const;
	Matrix4f GetViewProjection() const;
	const Vector3f& GetPosition() const;

	float GetPitch() const;
	float GetYaw() const;
	float GetRoll() const;

	void SetPosition(const Vector3f& position);
	void SetRotation(const Vector3f& rotation);

	Vector3f GetUpDirection() const;
	Vector3f GetRightDirection() const;
	Vector3f GetForwardDirection() const;

	ControllerStyle GetControllerStyle() const;
	void SetControllerStyle(ControllerStyle style);

private:
	bool OnMouseScroll(const MouseWheelScrolledEvent& event);

	void UpdateCameraView();

	float GetZoomSpeed() const;

private:
	bool _enabled = true;
	float _movementSpeed = 10.0f;
	ControllerStyle _controllerStyle = ControllerStyle::Game;

	Matrix4f _viewMatrix{};
	Vector3f _position{0.0f, 0.0f, 0.0f};
	Array<bool, 6> _keystates{false, false, false, false, false, false};

	Vector3f _forward{0.0f, 0.0f, 0.0f};
	Vector3f _up{0.0f, 0.0f, 0.0f};
	Vector3f _right{0.0f, 0.0f, 0.0f};
	Vector3f _worldUp{0.0f, 1.0f, 0.0f};

	float _pitch = 0.0f, _yaw = 0.0f;
};
}
