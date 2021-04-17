#include "SaffronPCH.h"

#include "Saffron/Core/App.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Editor/EditorCamera.h"

namespace Se
{
EditorCamera::EditorCamera() :
	EditorCamera(Matrix4f(1))
{
}

EditorCamera::EditorCamera(Matrix4f projectionMatrix) :
	Camera(Move(projectionMatrix))
{
	Reset();

	Mouse::WheelScrolled += SE_BIND_EVENT_FN(EditorCamera::OnMouseScroll);
}

void EditorCamera::OnUpdate()
{
	const auto ts = Global::Timer::GetStep();

	if (m_Enabled)
	{
		if (m_ControllerStyle == ControllerStyle::Maya)
		{
			if (Keyboard::IsDown(KeyCode::LAlt))
			{
				const Vector2f swipe = Mouse::GetSwipe() * ts.sec() * 0.7f;

				//if ( Input::IsMouseButtonPressed(SE_BUTTON_MIDDLE) )
				//	MousePan(swipe);
				//else if ( Input::IsMouseButtonPressed(SE_BUTTON_LEFT) )
				//	MouseRotate(swipe);
				//else if ( Input::IsMouseButtonPressed(SE_BUTTON_RIGHT) )
				//	MouseZoom(swipe.y);
			}
		}
		else if (m_ControllerStyle == ControllerStyle::Game)
		{
			if (Keyboard::IsDown(KeyCode::W))
			{
				m_Position += GetForwardDirection() * m_MovementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::S))
			{
				m_Position -= GetForwardDirection() * m_MovementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::A))
			{
				m_Position -= GetRightDirection() * m_MovementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::D))
			{
				m_Position += GetRightDirection() * m_MovementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::E))
			{
				m_Position.y += m_MovementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::Q))
			{
				m_Position.y -= m_MovementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::LShift))
			{
				m_MovementSpeed = 20.0f;
			}
			else
			{
				m_MovementSpeed = 10.0f;
			}

			if (Mouse::IsDown(MouseButtonCode::Right))
			{
				const Vector2f swipe = Mouse::GetSwipe() * ts.sec() * 0.3f;

				m_Yaw += swipe.x;
				m_Pitch -= swipe.y;
				m_Pitch = std::clamp(m_Pitch, -Math::PI / 2.0f + 0.01f, Math::PI / 2.0f - 0.01f);
			}
		}

		UpdateCameraView();
	}
}

void EditorCamera::OnGuiRender()
{
	ImGui::Begin("Editor Camera");
	Gui::BeginPropertyGrid();
	Gui::Property("Exposure", GetExposure(), 0.1f, 0.1f, 100.0f, Gui::PropertyFlag::Drag);
	Gui::EndPropertyGrid();
	ImGui::End();
}

void EditorCamera::Reset()
{
	m_Position = {0.0f, 10.0f, 10.0f};
	m_Yaw = 4 * Math::PI / 3;
	m_Pitch = -1 * Math::PI / 5.0f;
	UpdateCameraView();
}

void EditorCamera::Enable()
{
	m_Enabled = true;
}

void EditorCamera::Disable()
{
	m_Enabled = false;
}

bool EditorCamera::IsEnabled() const
{
	return m_Enabled;
}

const Matrix4f& EditorCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

Matrix4f EditorCamera::GetViewProjection() const
{
	return m_ProjectionMatrix * m_ViewMatrix;
}

const Vector3f& EditorCamera::GetPosition() const
{
	return m_Position;
}

float EditorCamera::GetPitch() const
{
	return m_Pitch;
}

float EditorCamera::GetYaw() const
{
	return m_Yaw;
}

float EditorCamera::GetRoll() const
{
	return 0.0f;
}


void EditorCamera::SetPosition(const Vector3f& position)
{
	m_Position = position;
	UpdateCameraView();
}

void EditorCamera::SetRotation(const Vector3f& rotation)
{
	m_Pitch = rotation.x;
	m_Yaw = rotation.y;
	m_Pitch = std::clamp(m_Pitch, -Math::PI / 2.0f + 0.01f, Math::PI / 2.0f - 0.01f);
	UpdateCameraView();
}

Vector3f EditorCamera::GetUpDirection() const
{
	return m_Up;
}

Vector3f EditorCamera::GetRightDirection() const
{
	return m_Right;
}

Vector3f EditorCamera::GetForwardDirection() const
{
	return m_Forward;
}

EditorCamera::ControllerStyle EditorCamera::GetControllerStyle() const
{
	return m_ControllerStyle;
}

void EditorCamera::SetControllerStyle(ControllerStyle style)
{
	m_ControllerStyle = style;
}

bool EditorCamera::OnMouseScroll(const MouseWheelScrolledEvent& event)
{
	const float delta = event.GetOffset() * 0.1f;
	//MouseZoom(delta);
	UpdateCameraView();
	return false;
}

void EditorCamera::UpdateCameraView()
{
	Vector3f front;
	front.x = std::cos(m_Yaw) * std::cos(m_Pitch);
	front.y = std::sin(m_Pitch);
	front.z = std::sin(m_Yaw) * std::cos(m_Pitch);
	m_Forward = normalize(front);

	m_Right = normalize(cross(m_Forward, m_WorldUp));
	m_Up = normalize(cross(m_Right, m_Forward));

	m_ViewMatrix = lookAt(m_Position, m_Position + m_Forward, m_Up);
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
