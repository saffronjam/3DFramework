#include "SaffronPCH.h"

#include "Saffron/Renderer/Camera.h"

namespace Se
{
Camera::Camera(const Matrix4f &projectionMatrix)
	: m_ProjectionMatrix(projectionMatrix)
{
}

void Camera::SetViewportSize(Uint32 width, Uint32 height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

}
