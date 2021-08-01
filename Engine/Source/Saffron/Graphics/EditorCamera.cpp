#include "SaffronPCH.h"

#include "Saffron/Graphics/EditorCamera.h"

namespace Se
{
const Vector3 EditorCamera::_worldUp(0.0f, 1.0f, 0.0f);

EditorCamera::EditorCamera() :
	Camera(Matrix::CreatePerspectiveFieldOfView(Math::Pi / 2.0f, 16.0f / 9.0f, 0.1f, 100.0f))
{
}

EditorCamera::EditorCamera(const Matrix& projection) :
	Camera(projection)
{
}

void EditorCamera::OnUpdate()
{
}

void EditorCamera::OnUi()
{
}

void EditorCamera::Reset()
{
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

	_forward.Cross(_worldUp).Normalize(_left);
	_left.Cross(_forward).Normalize(_up);

	_view = Matrix::CreateLookAt(_position, _position + _forward, _up);
}
}
