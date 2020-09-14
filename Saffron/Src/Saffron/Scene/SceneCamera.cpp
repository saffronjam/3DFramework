#include "Saffron/SaffronPCH.h"

#include "Saffron/Scene/SceneCamera.h"

namespace Se
{
void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
{
	m_ProjectionType = ProjectionType::Perspective;
	m_PerspectiveFOV = verticalFOV;
	m_PerspectiveNear = nearClip;
	m_PerspectiveFar = farClip;
}

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
	m_ProjectionType = ProjectionType::Orthographic;
	m_OrthographicSize = size;
	m_OrthographicNear = nearClip;
	m_OrthographicFar = farClip;
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
	switch ( m_ProjectionType )
	{
	case ProjectionType::Perspective:
		m_ProjectionMatrix = glm::perspectiveFov(m_PerspectiveFOV, static_cast<float>(width), static_cast<float>(height), m_PerspectiveNear, m_PerspectiveFar);
		break;
	case ProjectionType::Orthographic:
		const float aspect = static_cast<float>(width) / static_cast<float>(height);
		const float Width = m_OrthographicSize * aspect;
		const float Height = m_OrthographicSize;
		m_ProjectionMatrix = glm::ortho(-Width * 0.5f, Width * 0.5f, -Height * 0.5f, Height * 0.5f);
		break;
	}
}
}
