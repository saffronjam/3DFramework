#pragma once

#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Camera
{
public:
	explicit Camera(const glm::vec3 &position, const glm::mat4 &projection);
	virtual ~Camera() = default;

	const glm::vec3 &GetPosition() const;
	const glm::mat4 &GetProjectionMatrix() const;
	const glm::mat4 &GetViewMatrix() const;
	const glm::mat4 &GetViewProjectionMatrix() const;

	void SetPosition(const glm::vec3 &position);

protected:
	virtual	void RecalculateViewMatrix() = 0;

protected:
	glm::vec3 m_Position;

	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ViewProjectionMatrix;

};
}
