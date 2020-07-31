#pragma once

#include "Saffron/Renderer/OrthographicCamera.h"
#include "Saffron/System/Time.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Event/MouseEvent.h"
#include "Saffron/Event/WindowEvent.h"

namespace Se
{
class OrthographicCameraController
{
public:
	OrthographicCameraController(float aspectRatio, bool rotation = false);

	void OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts);
	void OnEvent(const Event &event);


	OrthographicCamera &GetCamera() { return m_Camera; }
	const OrthographicCamera &GetCamera() const { return m_Camera; }

	float GetZoomLevel() const { return m_ZoomLevel; }
	void SetZoomLevel(float level) { m_ZoomLevel = level; }

private:
	void OnWindowResize(const WindowResizeEvent &event);
	bool OnMouseScroll(const MouseScrollEvent &event);

private:
	float m_AspectRatio;
	float m_ZoomLevel;

	bool m_Rotation;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraTranslationSpeed, m_CameraRotationSpeed;
	OrthographicCamera m_Camera;
};
}
