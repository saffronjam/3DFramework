#pragma once

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
class Camera : public Managed
{
public:
	Camera() = default;
	explicit Camera(Matrix4f projectionMatrix);
	virtual ~Camera() = default;

	const Matrix4f& GetProjectionMatrix() const { return _projectionMatrix; }

	float GetExposure() const { return _exposure; }

	float& GetExposure() { return _exposure; }

	void SetProjectionMatrix(const Matrix4f& projectionMatrix) { _projectionMatrix = projectionMatrix; }

	virtual void SetViewportSize(Uint32 width, Uint32 height);

protected:
	Matrix4f _projectionMatrix = Matrix4f(1.0f);
	float _exposure = 0.8f;

	Uint32 _viewportWidth = 1280, _viewportHeight = 720;
};
}
