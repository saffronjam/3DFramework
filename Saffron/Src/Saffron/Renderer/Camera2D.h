#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Camera2D : public Camera
{
public:
	Camera2D(const glm::vec3 &position, float left, float right, float bottom, float top);
	virtual ~Camera2D() = default;

	void SetProjection(float left, float right, float bottom, float top);

	float GetRotation() const;
	void SetRotation(float rotation);

	void RecalculateViewMatrix() override;

private:
	float m_Rotation;
};
}
