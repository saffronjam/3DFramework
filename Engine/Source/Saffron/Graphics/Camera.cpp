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

void Camera::SetProjection(const struct ProjectionSpec& spec)
{
	_projectionSpec = spec;
	_projection = Matrix::CreatePerspectiveFieldOfView(spec.Fov, spec.AspectRatio, spec.Near, spec.Far);
}

auto Camera::ProjectionSpec() const -> const struct ProjectionSpec&
{
	return _projectionSpec;
}
}
