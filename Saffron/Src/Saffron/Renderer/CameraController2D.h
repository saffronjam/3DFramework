#pragma once

#include "Saffron/Renderer/Camera2D.h"
#include "Saffron/System/Time.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Core/Event/MouseEvent.h"
#include "Saffron/Core/Event/WindowEvent.h"

namespace Se
{
class CameraController2D
{
public:
	explicit CameraController2D(float aspectRatio, bool rotationEnabled = false);

	void OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts);
	void OnEvent(const Event &event);

	Camera2D &GetCamera();
	const Camera2D &GetCamera() const;

	float GetZoomLevel() const;
	void SetZoomLevel(float level);

private:
	void OnWindowResize(const WindowResizeEvent &event);
	void OnMouseScroll(const MouseScrollEvent &event);

private:
	float m_AspectRatio, m_ZoomLevel;
	bool m_RotationEnabled;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraTranslationSpeed, m_CameraRotationSpeed;
	Camera2D m_Camera;
};
}
