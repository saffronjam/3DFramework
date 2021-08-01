#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Camera.h"

namespace Se
{
class EditorCamera : public Camera
{
public:
	EditorCamera();
	explicit EditorCamera(const Matrix& projection);

	void OnUpdate();
	void OnUi();

	void Reset();

	auto View() const -> const Matrix&;

private:
	void CreateView();

private:
	Matrix _view;
	float _pitch = 0.0f, _yaw = 0.0f, _roll = 0.0f;
	Vector3 _position;
	Vector3 _forward, _left, _up;
	static const Vector3 _worldUp;
};
}
