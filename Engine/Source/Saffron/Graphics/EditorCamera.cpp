#include "SaffronPCH.h"

#include "Saffron/Graphics/EditorCamera.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Ui/Ui.h"

namespace Se
{
const Vector3 EditorCamera::_worldUp(0.0f, 1.0f, 0.0f);

EditorCamera::EditorCamera() :
	Camera(Matrix::CreatePerspectiveFieldOfView(Math::Pi / 4.0f, 16.0f / 9.0f, 0.1f, 100.0f))
{
	CreateView();
}

EditorCamera::EditorCamera(const Matrix& projection) :
	Camera(projection)
{
	CreateView();
}

void EditorCamera::OnUpdate(TimeSpan ts)
{
	const float speed = 100.0f;
	if (Keyboard::IsKeyDown(KeyCode::W))
	{
		_position += _forward * ts.Sec() * speed;
	}
	if (Keyboard::IsKeyDown(KeyCode::S))
	{
		_position -= _forward * ts.Sec() * speed;
	}
	if (Keyboard::IsKeyDown(KeyCode::D))
	{
		_position += _right * ts.Sec() * speed;
	}
	if (Keyboard::IsKeyDown(KeyCode::A))
	{
		_position -= _right * ts.Sec() * speed;
	}
	if (Keyboard::IsKeyDown(KeyCode::Q))
	{
		_position -= _worldUp * ts.Sec() * speed;
	}
	if (Keyboard::IsKeyDown(KeyCode::E))
	{
		_position += _worldUp * ts.Sec() * speed;
	}

	if (Mouse::IsButtonDown(MouseButtonCode::Right))
	{
		const Vector2 swipe = Mouse::Swipe() * ts.Sec() * 1.5f;

		_yaw -= swipe.x;
		_pitch += swipe.y;
		_pitch = std::clamp(_pitch, -Math::Pi / 2.0f + 0.01f, Math::Pi / 2.0f - 0.01f);
	}

	CreateView();
}

void EditorCamera::OnUi()
{
	ImGui::Begin("Camera");

	ImGui::Columns(2);

	ImGui::Text("Position");
	ImGui::NextColumn();
	ImGui::Text(std::format("{}", _position).c_str());
	ImGui::NextColumn();

	ImGui::Text("Roll, Pitch, Yaw");
	ImGui::NextColumn();
	ImGui::Text(std::format("{}", Vector3(_roll, _pitch, _yaw)).c_str());
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Forward");
	ImGui::NextColumn();
	ImGui::Text(std::format("{}", _forward).c_str());
	ImGui::NextColumn();

	ImGui::Text("Left");
	ImGui::NextColumn();
	ImGui::Text(std::format("{}", _right).c_str());
	ImGui::NextColumn();

	ImGui::Text("Up");
	ImGui::NextColumn();
	ImGui::Text(std::format("{}", _up).c_str());
	ImGui::NextColumn();

	ImGui::End();
}

void EditorCamera::Reset()
{
}

auto EditorCamera::Data() const -> CameraData
{
	return {View(), Projection(), _position};
}

auto EditorCamera::View() const -> const Matrix&
{
	return _view;
}

void EditorCamera::CreateView()
{
	Vector3 front;
	front.x = std::cos(_yaw) * std::cos(_pitch);
	front.y = std::sin(_pitch);
	front.z = std::sin(_yaw) * std::cos(_pitch);

	front.Normalize(_forward);

	_forward.Cross(_worldUp).Normalize(_right);
	_right.Cross(_forward).Normalize(_up);

	_view = Matrix::CreateLookAt(_position, _position + _forward, _up);
}
}
