#include "SaffronPCH.h"

#include "Saffron/Rendering/Camera.h"

namespace Se
{
Camera::Camera(Matrix4f projectionMatrix)
	: m_ProjectionMatrix(Move(projectionMatrix))
{
}


void Camera::SetViewportSize(Uint32 width, Uint32 height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

}
