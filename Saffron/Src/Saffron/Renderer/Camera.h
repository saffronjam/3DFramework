#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class Camera
{
public:
	Camera() = default;
	explicit Camera(const glm::mat4 &projectionMatrix);
	virtual ~Camera() = default;

	const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
	void SetProjectionMatrix(const glm::mat4 &projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

	virtual float GetExposure() const { return m_Exposure; }
	virtual float &GetExposure() { return m_Exposure; }
protected:
	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	float m_Exposure = 0.8f;
};
}
