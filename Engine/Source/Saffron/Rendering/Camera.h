#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Camera
{
public:
	Camera() = default;
	explicit Camera(Matrix projectionMatrix);
	virtual ~Camera() = default;

	const Matrix& GetProjectionMatrix() const;

	float GetExposure() const;
	float& GetExposure();

	void SetProjectionMatrix(const Matrix& projectionMatrix);
	virtual void SetViewportSize(Uint32 width, Uint32 height);

protected:
	Matrix _projectionMatrix = Matrix::Identity;
	float _exposure = 0.8f;

	Uint32 _viewportWidth = 1280, _viewportHeight = 720;
};
}
