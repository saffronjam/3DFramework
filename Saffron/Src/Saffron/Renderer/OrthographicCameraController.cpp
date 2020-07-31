#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/OrthographicCameraController.h"

namespace Se
{

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
	:
	m_AspectRatio(aspectRatio),
	m_ZoomLevel(1.0f),
	m_Rotation(rotation),
	m_CameraPosition(0.0f, 0.0f, 0.0f),
	m_CameraRotation(0.0f),
	m_CameraTranslationSpeed(5.0f),
	m_CameraRotationSpeed(180.0f),
	m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio *m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
{
}

void OrthographicCameraController::OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts)
{
	//SE_PROFILE_FUNCTION();

	if ( keyboard.IsDown(SE_KEY_A) )
	{
		m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
		m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
	}
	else if ( keyboard.IsDown(SE_KEY_D) )
	{
		m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
		m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
	}

	if ( keyboard.IsDown(SE_KEY_W) )
	{
		m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
		m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
	}
	else if ( keyboard.IsDown(SE_KEY_S) )
	{
		m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
		m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts.sec();
	}

	if ( m_Rotation )
	{
		if ( keyboard.IsDown(SE_KEY_Q) )
			m_CameraRotation += m_CameraRotationSpeed * ts.sec();
		if ( keyboard.IsDown(SE_KEY_E) )
			m_CameraRotation -= m_CameraRotationSpeed * ts.sec();

		m_CameraRotation = std::clamp(m_CameraRotation, -180.0f, 180.0f);
		m_Camera.SetRotation(m_CameraRotation);
	}

	m_Camera.SetPosition(m_CameraPosition);

	m_CameraTranslationSpeed = m_ZoomLevel;
}

void OrthographicCameraController::OnEvent(const Event &e)
{
	//SE_PROFILE_FUNCTION();

	const EventDispatcher dispatcher(e);
	dispatcher.Try<MouseScrollEvent>(SE_EVENT_FN(OrthographicCameraController::OnMouseScroll));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(OrthographicCameraController::OnWindowResize));
}

void OrthographicCameraController::OnWindowResize(const WindowResizeEvent &event)
{
	m_AspectRatio = static_cast<float>(event.GetWidth()) / static_cast<float>(event.GetHeight());
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

bool OrthographicCameraController::OnMouseScroll(const MouseScrollEvent &event)
{
	//SE_PROFILE_FUNCTION();

	m_ZoomLevel -= event.GetOffsetY() * 0.25f;
	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	return false;
}

}
