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
	void SetControllerStyle(ControllerStyle style) { m_ControllerStyle = style; }

private:
	bool OnMouseScroll(const MouseScrollEvent &event);

	void MousePan(const glm::vec2 &delta);
	void MouseRotate(const glm::vec2 &delta);
	void MouseZoom(float delta);

	glm::vec2 GetPanSpeed() const;
	float GetRotationSpeed() const;
	float GetZoomSpeed() const;

private:
	float m_MovementSpeed = 10.0f;
	ControllerStyle	m_ControllerStyle = ControllerStyle::Maya;

};
}
