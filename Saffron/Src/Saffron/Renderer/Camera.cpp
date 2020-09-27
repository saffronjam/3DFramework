#include "SaffronPCH.h"

#include "Saffron/Renderer/Camera.h"

namespace Se
{
Camera::Camera(const glm::mat4 &projectionMatrix)
	: m_ProjectionMatrix(projectionMatrix)
{
}

glm::vec3 Camera::GetUpDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::GetRightDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Camera::GetForwardDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::quat Camera::GetOrientation() const
{
	return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}

void Camera::SetViewportSize(Uint32 width, Uint32 height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

void Camera::UpdateCameraView()
{
	m_Position = CalculatePosition();

	const glm::quat orientation = GetOrientation();
	m_Rotation = glm::eulerAngles(orientation) * (180.0f / Math::PI);
	m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
	m_ViewMatrix = glm::inverse(m_ViewMatrix);
}

glm::vec3 Camera::CalculatePosition() const
{
	return m_FocalPoint - GetForwardDirection() * m_Distance;
}
}
