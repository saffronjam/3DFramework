#include "SaffronPCH.h"

#include "Saffron/Input/Input.h"
#include "Saffron/Editor/EditorCamera.h"

namespace Se
{
EditorCamera::EditorCamera(const ViewportPane &mainViewport)
	: m_EditorViewport(mainViewport)
{
	Reset();
}

EditorCamera::EditorCamera(const ViewportPane &mainViewport, const glm::mat4 &projectionMatrix)
	: Camera(projectionMatrix),
	m_EditorViewport(mainViewport)
{
	Reset();
}

void EditorCamera::OnUpdate(Time ts)
{
	if ( m_EditorViewport.IsFocused() )
	{
		if ( m_ControllerStyle == ControllerStyle::Maya )
		{
			if ( Input::IsKeyPressed(KeyCode::LeftAlt) )
			{
				const glm::vec2 swipe = Input::GetMouseSwipe() * ts.sec() * 0.7f;

				//if ( Input::IsMouseButtonPressed(SE_BUTTON_MIDDLE) )
				//	MousePan(swipe);
				//else if ( Input::IsMouseButtonPressed(SE_BUTTON_LEFT) )
				//	MouseRotate(swipe);
				//else if ( Input::IsMouseButtonPressed(SE_BUTTON_RIGHT) )
				//	MouseZoom(swipe.y);
			}
		}
		else if ( m_ControllerStyle == ControllerStyle::Game )
		{
			if ( Input::IsKeyPressed(SE_KEY_W) )
			{
				m_Position += GetForwardDirection() * m_MovementSpeed * ts.sec();
			}

			if ( Input::IsKeyPressed(SE_KEY_S) )
			{
				m_Position -= GetForwardDirection() * m_MovementSpeed * ts.sec();
			}

			if ( Input::IsKeyPressed(SE_KEY_A) )
			{
				m_Position -= GetRightDirection() * m_MovementSpeed * ts.sec();
			}

			if ( Input::IsKeyPressed(SE_KEY_D) )
			{
				m_Position += GetRightDirection() * m_MovementSpeed * ts.sec();
			}

			if ( Input::IsKeyPressed(SE_KEY_E) )
			{
				m_Position.y += m_MovementSpeed * ts.sec();
			}

			if ( Input::IsKeyPressed(SE_KEY_Q) )
			{
				m_Position.y -= m_MovementSpeed * ts.sec();
			}

			if ( Input::IsKeyPressed(SE_KEY_LEFT_SHIFT) )
			{
				m_MovementSpeed = 20.0f;
			}
			else
			{
				m_MovementSpeed = 10.0f;
			}

			if ( Input::IsMouseButtonPressed(SE_BUTTON_RIGHT) )
			{
				const glm::vec2 swipe = Input::GetMouseSwipe() * ts.sec() * 0.3f;

				m_Yaw += swipe.x;
				m_Pitch -= swipe.y;
				m_Pitch = std::clamp(m_Pitch, -Math::PI / 2.0f + 0.01f, Math::PI / 2.0f - 0.01f);
			}
		}

	}
	UpdateCameraView();
}

bool EditorCamera::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<MouseScrollEvent>(SE_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	return false;
}

void EditorCamera::Reset()
{
	m_Position = { 0.0f, 10.0f, 10.0f };
	m_Yaw = 4 * Math::PI / 3;
	m_Pitch = -1 * Math::PI / 5.0f;
	UpdateCameraView();
}


glm::vec3 EditorCamera::GetUpDirection() const
{
	return m_Up;
}

glm::vec3 EditorCamera::GetRightDirection() const
{
	return m_Right;
}

glm::vec3 EditorCamera::GetForwardDirection() const
{
	return m_Forward;
}

bool EditorCamera::OnMouseScroll(const MouseScrollEvent &event)
{
	const float delta = event.GetOffsetY() * 0.1f;
	//MouseZoom(delta);
	UpdateCameraView();
	return false;
}

void EditorCamera::UpdateCameraView()
{
	glm::vec3 front;
	front.x = std::cos(m_Yaw) * std::cos(m_Pitch);
	front.y = std::sin(m_Pitch);
	front.z = std::sin(m_Yaw) * std::cos(m_Pitch);
	m_Forward = glm::normalize(front);

	m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Forward));

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

float EditorCamera::GetZoomSpeed() const
{
	//float distance = m_Distance * 0.2f;
	//distance = std::max(distance, 0.0f);
	//float speed = distance * distance;
	//speed = std::min(speed, 100.0f); // max speed = 100
	//return speed;

	return m_MovementSpeed;
}
}
