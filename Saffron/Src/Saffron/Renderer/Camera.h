#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class Camera : public RefCounted
{
public:
	Camera() = default;
	explicit Camera(const glm::mat4 &projectionMatrix);
	virtual ~Camera() = default;

	const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }

	float GetExposure() const { return m_Exposure; }
	float &GetExposure() { return m_Exposure; }

	void SetProjectionMatrix(const glm::mat4 &projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
	virtual void SetViewportSize(Uint32 width, Uint32 height);

protected:
	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	float m_Exposure = 0.8f;

	Uint32 m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}
