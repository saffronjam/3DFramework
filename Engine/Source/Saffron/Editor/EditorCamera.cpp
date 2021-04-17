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

	if (_enabled)
	{
		if (_controllerStyle == ControllerStyle::Maya)
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
				_position -= GetRightDirection() * _movementSpeed * ts.sec();
			}

			if (Keyboard::IsDown(KeyCode::D))
			{
				_position += GetRightDirection() * _movementSpeed * ts.sec();
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
				_movementSpeed = 20.0f;
			}
			else
			{
				_movementSpeed = 10.0f;
			}

			if (Mouse::IsDown(MouseButtonCode::Right))
			{
				const Vector2f swipe = Mouse::GetSwipe() * ts.sec() * 0.3f;

				_yaw += swipe.x;
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
	Gui::BeginPropertyGrid();
	Gui::Property("Exposure", GetExposure(), 0.1f, 0.1f, 100.0f, Gui::PropertyFlag::Drag);
	Gui::EndPropertyGrid();
	ImGui::End();
}

void EditorCamera::Reset()
{
	_position = {0.0f, 10.0f, 10.0f};
	_yaw = 4 * Math::PI / 3;
	_pitch = -1 * Math::PI / 5.0f;
	UpdateCameraView();
}

void EditorCamera::Enable()
{
	_enabled = true;
}

void EditorCamera::Disable()
{
	_enabled = false;
}

bool EditorCamera::IsEnabled() const
{
	return _enabled;
}

const Matrix4f& EditorCamera::GetViewMatrix() const
{
	return _viewMatrix;
}

Matrix4f EditorCamera::GetViewProjection() const
{
	return _projectionMatrix * _viewMatrix;
}

const Vector3f& EditorCamera::GetPosition() const
{
	return _position;
}

float EditorCamera::GetPitch() const
{
	return _pitch;
}

float EditorCamera::GetYaw() const
{
	return _yaw;
}

float EditorCamera::GetRoll() const
{
	return 0.0f;
}


void EditorCamera::SetPosition(const Vector3f& position)
{
	_position = position;
	UpdateCameraView();
}

void EditorCamera::SetRotation(const Vector3f& rotation)
{
	_pitch = rotation.x;
	_yaw = rotation.y;
	_pitch = std::clamp(_pitch, -Math::PI / 2.0f + 0.01f, Math::PI / 2.0f - 0.01f);
	UpdateCameraView();
}

Vector3f EditorCamera::GetUpDirection() const
{
	return _up;
}

Vector3f EditorCamera::GetRightDirection() const
{
	return _right;
}

Vector3f EditorCamera::GetForwardDirection() const
{
	return _forward;
}

EditorCamera::ControllerStyle EditorCamera::GetControllerStyle() const
{
	return _controllerStyle;
}

void EditorCamera::SetControllerStyle(ControllerStyle style)
{
	_controllerStyle = style;
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
	front.x = std::cos(_yaw) * std::cos(_pitch);
	front.y = std::sin(_pitch);
	front.z = std::sin(_yaw) * std::cos(_pitch);
	_forward = normalize(front);

	_right = normalize(cross(_forward, _worldUp));
	_up = normalize(cross(_right, _forward));

	_viewMatrix = lookAt(_position, _position + _forward, _up);
}

float EditorCamera::GetZoomSpeed() const
{
	//float distance = _distance * 0.2f;
	//distance = std::max(distance, 0.0f);
	//float speed = distance * distance;
	//speed = std::min(speed, 100.0f); // max speed = 100
	//return speed;

	return _movementSpeed;
}
}
