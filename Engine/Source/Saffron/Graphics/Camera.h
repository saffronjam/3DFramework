#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Camera
{
public:
	Camera();
	explicit Camera(const Matrix& projection);

	auto Projection() const -> const Matrix&;
	void SetProjection(const Matrix& projection);

private:
	Matrix _projection;
};
}
