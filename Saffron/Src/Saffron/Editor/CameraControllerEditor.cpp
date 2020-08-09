#include "Saffron/SaffronPCH.h"

#include "CameraControllerEditor.h"

namespace Se
{

CameraControllerEditor::CameraControllerEditor(float aspectRatio)
	:
	m_AspectRatio(aspectRatio),
	m_ZoomLevel(1.0f),
	m_CameraPosition(0.0f, 0.0f, 3.0f),
	m_CameraYaw(-90.0f),
	m_CameraPitch(0.0f),
	m_CameraTranslationSpeed(3.0f),
	m_MouseSensitivity(0.13f),
	m_Camera(m_CameraPosition, glm::perspective(45.0f, m_AspectRatio, 100.0f, 0.1f))
{
	m_Camera.SetPosition(m_CameraPosition);
	m_Camera.SetYaw(m_CameraYaw);
	m_Camera.SetPitch(m_CameraPitch);
}

void CameraControllerEditor::OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts)
{
	SE_PROFILE_FUNCTION();

	const float multiplier = keyboard.IsDown(SE_KEY_LEFT_SHIFT) ? 2.5f : 1.0f;
	const float velocity = m_CameraTranslationSpeed * multiplier * ts.sec();
	if ( keyboard.IsDown(SE_KEY_W) )
		m_CameraPosition += m_Camera.GetForward() * velocity;
	if ( keyboard.IsDown(SE_KEY_S) )
		m_CameraPosition -= m_Camera.GetForward() * velocity;
	if ( keyboard.IsDown(SE_KEY_A) )
		m_CameraPosition -= m_Camera.GetRight() * velocity;
	if ( keyboard.IsDown(SE_KEY_D) )
		m_CameraPosition += m_Camera.GetRight() * velocity;

	m_Camera.SetPosition(m_CameraPosition);

	// Handle yaw and pitch
	if ( mouse.IsAnyDown() )
	{
		const glm::vec2 mouseDelta = mouse.GetDelta() * m_MouseSensitivity;

		m_CameraYaw -= mouseDelta.x;
		m_CameraPitch += mouseDelta.y;
		m_CameraPitch = std::clamp(m_CameraPitch, -89.0f, 89.0f);
		m_Camera.SetYaw(m_CameraYaw);
		m_Camera.SetPitch(m_CameraPitch);
	}
}

void CameraControllerEditor::OnEvent(const Event &event)
{
	SE_PROFILE_FUNCTION();

	const EventDispatcher dispatcher(event);
	dispatcher.Try<MouseScrollEvent>(SE_EVENT_FN(CameraControllerEditor::OnMouseScroll));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(CameraControllerEditor::OnWindowResize));
}

Camera3D &CameraControllerEditor::GetCamera()
{
	return m_Camera;
}

const Camera3D &CameraControllerEditor::GetCamera() const
{
	return m_Camera;
}

float CameraControllerEditor::GetZoomLevel() const
{
	return m_ZoomLevel;
}

void CameraControllerEditor::SetZoomLevel(float level)
{
	m_ZoomLevel = level;
}

void CameraControllerEditor::OnWindowResize(const WindowResizeEvent &event)
{
	SE_PROFILE_FUNCTION();

	m_AspectRatio = static_cast<float>(event.GetWidth()) / static_cast<float>(event.GetHeight());
	m_Camera.SetProjection(glm::perspective(45.0f, m_AspectRatio, 100.0f, 0.1f));
}

void CameraControllerEditor::OnMouseScroll(const MouseScrollEvent &event)
{
	SE_PROFILE_FUNCTION();

	m_ZoomLevel -= event.GetOffsetY() * 0.25f;
	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
}

}
