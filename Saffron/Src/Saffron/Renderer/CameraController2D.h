#pragma once

#include "Saffron/Renderer/Camera2D.h"
#include "Saffron/System/Time.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Event/MouseEvent.h"
#include "Saffron/Event/WindowEvent.h"

namespace Se
{
class CameraController2D
{
public:
	CameraController2D(float aspectRatio, bool rotationEnabled = false);

	void OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts);
	void OnEvent(const Event &event);

	Camera2D &GetCamera() { return m_Camera; }
	const Camera2D &GetCamera() const { return m_Camera; }

	float GetZoomLevel() const { return m_ZoomLevel; }
	void SetZoomLevel(float level) { m_ZoomLevel = level; }

private:
	void OnWindowResize(const WindowResizeEvent &event);
	void OnMouseScroll(const MouseScrollEvent &event);

private:
	float m_AspectRatio;
	float m_ZoomLevel;

	bool m_RotationEnabled;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraTranslationSpeed, m_CameraRotationSpeed;
	Camera2D m_Camera;
};
}
