#include "SaffronPCH.h"

#include "Saffron/Common/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

namespace Se
{
EditorCamera::EditorCamera() :
	Camera(Matrix::CreatePerspectiveFieldOfView(Math::PI / 2, 16.0f / 9.0f, 0.1f, 10000.0f))
{
	Reset();
}

EditorCamera::EditorCamera(Matrix projectionMatrix) :
	Camera(Move(projectionMatrix))
{
	SetPosition(Vector3{0.0f, 0.0f, -5.0f});
	Reset();
}

void EditorCamera::OnUpdate()
{
	const auto ts = Global::Timer::GetStep();

	if (_enabled)
	{
		if (_controllerStyle == ControllerStyle::Maya)
		{
			if (Keyboard::IsPressed(KeyCode::LAlt))
			{
				const Vector2 swipe = Mouse::GetSwipe() * ts.sec() * 0.7f;

				//if ( Input::IsMouseButtonPressed(SE_BUTTON_MIDDLE) )
				//	MousePan(swipe);
				//else if ( Input::IsMouseButtonPressed(SE_BUTTON_LEFT) )
				//	MouseRotate(swipe);
				//else if ( Input::IsMouseButtonPressed(SE_BUTTON_RIGHT) )
				//	MouseZoom(swipe.y);
			}
		}
		else if (_controllerStyle == ControllerStyle::Game)
		{
			if (Keyboard::IsDown(KeyCode::W))
			{
				_position += GetForwardDirection() * _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::S))
			{
				_position -= GetForwardDirection() * _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::A))
			{
				_position += GetLeftDirection() * _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::D))
			{
				_position -= GetLeftDirection() * _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::E))
			{
				_position.y += _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::Q))
			{
				_position.y -= _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::LShift))
			{
				_movementSpeed = 1.0f;
			}
			else
			{
				_movementSpeed = 10.0f;
			}
			
			if (Mouse::IsDown(MouseButtonCode::Right))
			{
				const Vector2 swipe = Mouse::GetSwipe() * ts.sec() * 0.3f;
				
				m_Yaw -= swipe.x;
				_pitch -= swipe.y;
				_pitch = std::clamp(_pitch, -Math::PI / 2.0f + 0.01f, Math::PI / 2.0f - 0.01f);
			}
		}

		UpdateCameraView();
	}
}

void EditorCamera::OnGuiRender()
{
	ImGui::Begin("Editor Camera");
	//Gui::BeginPropertyGrid();
	//Gui::Property("Exposure", GetExposure(), 0.0f, 100.0f, 0.1f, Gui::PropertyFlag::Drag);
	//Gui::EndPropertyGrid();
	ImGui::End();
}

bool EditorCamera::OnEvent(const Event& event)
{
	if (_enabled)
	{
		const EventDispatcher dispatcher(event);
		dispatcher.Try<MouseWheelScrolledEvent>(SE_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}
	return false;
}

void EditorCamera::Reset()
{
	_position = Vector3{0.0f, 10.0f, 10.0f};
	m_Yaw = 4 * Math::PI / 3;
	_pitch = -1 * Math::PI / 5.0f;
	UpdateCameraView();
}


void EditorCamera::SetPosition(const Vector3& position)
{
	_position = position;
	UpdateCameraView();
}

void EditorCamera::SetRotation(const Vector3& rotation)
{
	_pitch = rotation.x;
	m_Yaw = rotation.y;
	_pitch = std::clamp(_pitch, -Math::PI / 2.0f + 0.01f, Math::PI / 2.0f - 0.01f);
	UpdateCameraView();
}

Vector3 EditorCamera::GetUpDirection() const
{
	return _up;
}

Vector3 EditorCamera::GetLeftDirection() const
{
	return _left;
}

Vector3 EditorCamera::GetForwardDirection() const
{
	return _forward;
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
	Vector3 front;
	front.x = std::cos(m_Yaw) * std::cos(_pitch);
	front.y = std::sin(_pitch);
	front.z = std::sin(m_Yaw) * std::cos(_pitch);

	front.Normalize(_forward);

	_forward.Cross(_worldUp).Normalize(_left);
	_left.Cross(_forward).Normalize(_up);

	_viewMatrix = Matrix::CreateLookAt(_position, _position + _forward, _up);
}

float EditorCamera::GetZoomSpeed() const
{
	//float distance = m_Distance * 0.2f;
	//distance = std::max(distance, 0.0f);
	//float speed = distance * distance;
	//speed = std::min(speed, 100.0f); // max speed = 100
	//return speed;

	return _movementSpeed;
}
}
