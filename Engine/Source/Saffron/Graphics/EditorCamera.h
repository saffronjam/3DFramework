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

	void OnUpdate(TimeSpan ts);
	void OnUi();

	void Reset();

	auto Data() const -> CameraData override;	
	auto View() const -> const Matrix&;

private:
	void CreateView();

private:
	Matrix _view;
	float _pitch = 0.0f, _yaw = 0.0f, _roll = 0.0f;
	Vector3 _position = {-1.0f, 0.0f, 0.0f};
	Vector3 _forward{0.0f, 0.0f, 1.0f}, _right = {1.0f, 0.0f, 0.0f}, _up{0.0f, 1.0f, 0.0f};
	static const Vector3 _worldUp;
};
}
