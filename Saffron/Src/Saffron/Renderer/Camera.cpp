#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Camera.h"

namespace Se
{
Camera::Camera(const glm::vec3 &position, const glm::mat4 &projection)
	:
	m_Position(position),
	m_ProjectionMatrix(projection),
	m_ViewMatrix(1.0f),
	m_ViewProjectionMatrix(m_ProjectionMatrix *m_ViewMatrix)
{
}

const glm::vec3 &Camera::GetPosition() const
{
	return m_Position;
}

const glm::mat4 &Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const glm::mat4 &Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const glm::mat4 &Camera::GetViewProjectionMatrix() const
{
	return m_ViewProjectionMatrix;
}

void Camera::SetPosition(const glm::vec3 &position)
{
	m_Position = position;
	RecalculateViewMatrix();
}

}
