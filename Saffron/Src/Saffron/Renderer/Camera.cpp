#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Camera.h"

namespace Se
{
Camera::Camera(const glm::mat4 &projectionMatrix)
	: m_ProjectionMatrix(projectionMatrix)
{
}
}
