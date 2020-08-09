#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Camera2D.h"

namespace Se
{

Camera2D::Camera2D(const glm::vec3 &position, float left, float right, float bottom, float top)
	:
	Camera(position, glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
	m_Rotation(0.0f)
{
	SE_PROFILE_FUNCTION();
	Camera2D::CalculateViewMatrix();
}

void Camera2D::SetProjection(float left, float right, float bottom, float top)
{
	SE_PROFILE_FUNCTION();

	Camera::SetProjection(glm::ortho(left, right, bottom, top, -1.0f, 1.0f));
}

float Camera2D::GetRotation() const
{
	return m_Rotation;
}

void Camera2D::SetRotation(float rotation)
{
	m_Rotation = rotation;
	CalculateViewMatrix();
}

void Camera2D::CalculateViewMatrix()
{
	SE_PROFILE_FUNCTION();

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

}
