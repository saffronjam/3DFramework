#include "SaffronPCH.h"

#include "Saffron/Rendering/Camera.h"

namespace Se
{
Camera::Camera(Matrix4 projectionMatrix) :
	_projectionMatrix(Move(projectionMatrix))
{
}


void Camera::SetViewportSize(uint width, uint height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}
}
