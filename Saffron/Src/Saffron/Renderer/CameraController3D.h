#pragma once

#include "Saffron/Renderer/Camera3D.h"
#include "Saffron/System/Time.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Event/MouseEvent.h"
#include "Saffron/Event/WindowEvent.h"

namespace Se
{
class CameraController3D
{
public:
	explicit CameraController3D(float aspectRatio);

	void OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts);
	void OnEvent(const Event &event);

	Camera3D &GetCamera();
	const Camera3D &GetCamera() const;

	float GetZoomLevel() const;
	void SetZoomLevel(float level);

private:
	void OnWindowResize(const WindowResizeEvent &event);
	void OnMouseScroll(const MouseScrollEvent &event);

private:
	float m_AspectRatio, m_ZoomLevel;

	glm::vec3 m_CameraPosition;
	float m_CameraYaw, m_CameraPitch;
	float m_CameraTranslationSpeed, m_MouseSensitivity;
	Camera3D m_Camera;
};
}
