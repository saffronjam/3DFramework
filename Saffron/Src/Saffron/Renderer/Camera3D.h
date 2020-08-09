#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Camera3D : public Camera
{
public:
	Camera3D(const glm::vec3 &position, const glm::mat4 &projection);
	virtual ~Camera3D() = default;

	float GetYaw() const;
	float GetPitch() const;
	const glm::vec3 &GetForward() const;
	const glm::vec3 &GetRight() const;

	void SetYaw(float yaw);
	void SetPitch(float pitch);

	void CalculateViewMatrix() override;

private:
	glm::vec3 m_Forward, m_Up, m_Right, m_WorldUp;
	float m_Yaw, m_Pitch;
};
}
