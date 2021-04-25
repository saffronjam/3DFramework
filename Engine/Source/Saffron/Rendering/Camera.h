#pragma once

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
class Camera : public Managed
{
public:
	Camera() = default;
	explicit Camera(Matrix4 projectionMatrix);
	virtual ~Camera() = default;

	const Matrix4& GetProjectionMatrix() const { return _projectionMatrix; }

	float GetExposure() const { return _exposure; }

	float& GetExposure() { return _exposure; }

	void SetProjectionMatrix(const Matrix4& projectionMatrix) { _projectionMatrix = projectionMatrix; }

	virtual void SetViewportSize(uint width, uint height);

protected:
	Matrix4 _projectionMatrix = Matrix4(1.0f);
	float _exposure = 0.8f;

	uint _viewportWidth = 1280, _viewportHeight = 720;
};
}
