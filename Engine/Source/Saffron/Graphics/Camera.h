#pragma once

#include "Saffron/Base.h"

namespace Se
{
struct CameraData
{
	Matrix View;
	Matrix Projection;
};

class Camera
{
public:
	Camera();
	explicit Camera(const Matrix& projection);
	virtual ~Camera() = default;

	virtual auto Data() const -> CameraData = 0;

	auto Projection() const -> const Matrix&;
	void SetProjection(const Matrix& projection);

private:
	Matrix _projection;
};
}
