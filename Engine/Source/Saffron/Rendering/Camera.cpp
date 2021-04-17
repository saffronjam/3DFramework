#include "SaffronPCH.h"

#include "Saffron/Rendering/Camera.h"

namespace Se
{
Camera::Camera(Matrix4f projectionMatrix) :
	_projectionMatrix(Move(projectionMatrix))
{
}


void Camera::SetViewportSize(Uint32 width, Uint32 height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}
}
