#include "SaffronPCH.h"

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Scene/SceneCamera.h"

namespace Se
{
SceneCamera::SceneCamera(Uint32 width, Uint32 height, ProjectionType projectionType)
{
	SceneCamera::SetViewportSize(width, height);
}

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

void SceneCamera::SetViewportSize(Uint32 width, Uint32 height)
{
	Camera::SetViewportSize(width, height);
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
