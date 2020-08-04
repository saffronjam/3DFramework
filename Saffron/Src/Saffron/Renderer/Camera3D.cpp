#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Camera3D.h"

namespace Se
{
Camera3D::Camera3D(const glm::vec3 &position, float fov, float aspect, float nearClip, float farClip)
	: Camera(position, glm::perspective(fov, aspect, nearClip, farClip)),
	m_Forward(0.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_WorldUp(0.0f, 1.0f, 0.0f),
	m_Yaw(-90.0f),
	m_Pitch(0.0f)
{
	SE_PROFILE_FUNCTION();

	Camera3D::RecalculateViewMatrix();
}

void Camera3D::SetProjection(float fov, float aspect, float nearClip, float farClip)
{
	SE_PROFILE_FUNCTION();

	m_ProjectionMatrix = glm::perspective(fov, aspect, nearClip, farClip);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

float Camera3D::GetYaw() const
{
	return m_Yaw;
}

float Camera3D::GetPitch() const
{
	return m_Pitch;
}

const glm::vec3 &Camera3D::GetForward() const
{
	return m_Forward;
}

const glm::vec3 &Camera3D::GetRight() const
{
	return m_Right;
}

void Camera3D::SetYaw(float yaw)
{
	m_Yaw = yaw;
	RecalculateViewMatrix();
}

void Camera3D::SetPitch(float pitch)
{
	m_Pitch = pitch;
	RecalculateViewMatrix();
}

void Camera3D::RecalculateViewMatrix()
{
	SE_PROFILE_FUNCTION();

	glm::vec3 front;
	front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
	front.y = std::sin(glm::radians(m_Pitch));
	front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
	m_Forward = glm::normalize(front);

	m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Forward));

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
}
