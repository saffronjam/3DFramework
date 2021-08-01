#include "SaffronPCH.h"

#include "Saffron/Graphics/Camera.h"

namespace Se
{
Camera::Camera() :
	_projection(Matrix::Identity)
{
}

Camera::Camera(const Matrix& projection) :
	_projection(projection)
{
}

auto Camera::Projection() const -> const Matrix&
{
	return _projection;
}

void Camera::SetProjection(const Matrix& projection)
{
	_projection = projection;
}
}
