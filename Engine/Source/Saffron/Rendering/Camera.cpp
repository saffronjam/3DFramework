#include "SaffronPCH.h"

#include "Saffron/Rendering/Camera.h"

namespace Se
{
Camera::Camera(Matrix projectionMatrix) :
	_projectionMatrix(Move(projectionMatrix))
{
}

const Matrix& Camera::GetProjectionMatrix() const
{
	return _projectionMatrix;
}

float Camera::GetExposure() const
{
	return _exposure;
}

float& Camera::GetExposure()
{
	return _exposure;
}

void Camera::SetProjectionMatrix(const Matrix& projectionMatrix)
{
	_projectionMatrix = projectionMatrix;
}


void Camera::SetViewportSize(Uint32 width, Uint32 height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}
}
