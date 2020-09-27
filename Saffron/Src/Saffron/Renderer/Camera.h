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
	void SetProjectionMatrix(const glm::mat4 &projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

	float GetDistance() const { return m_Distance; }

	const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
	glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;
	const glm::vec3 &GetPosition() const { return m_Position; }
	glm::quat GetOrientation() const;
	float GetExposure() const { return m_Exposure; }
	float &GetExposure() { return m_Exposure; }
	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }

	void SetDistance(float distance) { m_Distance = distance; }
	virtual void SetViewportSize(Uint32 width, Uint32 height);

protected:
	void UpdateCameraView();
	glm::vec3 CalculatePosition() const;

protected:
	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	float m_Exposure = 0.8f;

	glm::mat4 m_ViewMatrix{};
	glm::vec3 m_Position{}, m_Rotation{}, m_FocalPoint{};

	float m_Distance{};
	float m_Pitch{}, m_Yaw{};

	Uint32 m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}
