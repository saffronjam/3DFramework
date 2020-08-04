#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/CameraController2D.h"

namespace Se
{

CameraController2D::CameraController2D(float aspectRatio, bool rotationEnabled)
	:
	m_AspectRatio(aspectRatio),
	m_ZoomLevel(1.0f),
	m_RotationEnabled(rotationEnabled),
	m_CameraPosition(0.0f, 0.0f, 0.0f),
	m_CameraRotation(0.0f),
	m_CameraTranslationSpeed(5.0f),
	m_CameraRotationSpeed(180.0f),
	m_Camera(m_CameraPosition, -m_AspectRatio * m_ZoomLevel, m_AspectRatio *m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
{
}

void CameraController2D::OnUpdate(const Keyboard &keyboard, const Mouse &mouse, Time ts)
{
	SE_PROFILE_FUNCTION();

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

	if ( m_RotationEnabled )
	{
		if ( keyboard.IsDown(SE_KEY_Q) )
			m_CameraRotation += m_CameraRotationSpeed * ts.sec();
		if ( keyboard.IsDown(SE_KEY_E) )
			m_CameraRotation -= m_CameraRotationSpeed * ts.sec();

		if ( m_CameraRotation < -180.0f )
		{
			m_CameraRotation += 360.0f;
		}
		else if ( m_CameraRotation > 180.0f )
		{
			m_CameraRotation -= 360.0f;
		}
		m_Camera.SetRotation(m_CameraRotation);
	}

	m_Camera.SetPosition(m_CameraPosition);

	m_CameraTranslationSpeed = m_ZoomLevel;
}

void CameraController2D::OnEvent(const Event &event)
{
	SE_PROFILE_FUNCTION();

	const EventDispatcher dispatcher(event);
	dispatcher.Try<MouseScrollEvent>(SE_EVENT_FN(CameraController2D::OnMouseScroll));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(CameraController2D::OnWindowResize));
}

Camera2D &CameraController2D::GetCamera()
{
	return m_Camera;
}

const Camera2D &CameraController2D::GetCamera() const
{
	return m_Camera;
}

float CameraController2D::GetZoomLevel() const
{
	return m_ZoomLevel;
}

void CameraController2D::SetZoomLevel(float level)
{
	m_ZoomLevel = level;
}

void CameraController2D::OnWindowResize(const WindowResizeEvent &event)
{
	m_AspectRatio = static_cast<float>(event.GetWidth()) / static_cast<float>(event.GetHeight());
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

void CameraController2D::OnMouseScroll(const MouseScrollEvent &event)
{
	SE_PROFILE_FUNCTION();

	m_ZoomLevel -= event.GetOffsetY() * 0.25f;
	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

}
