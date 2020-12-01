#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class Camera : public MemManaged<Camera>
{
public:
	Camera() = default;
	explicit Camera(Matrix4f projectionMatrix);
	virtual ~Camera() = default;

	const Matrix4f &GetProjectionMatrix() const { return m_ProjectionMatrix; }

	float GetExposure() const { return m_Exposure; }
	float &GetExposure() { return m_Exposure; }

	void SetProjectionMatrix(const Matrix4f &projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
	virtual void SetViewportSize(Uint32 width, Uint32 height);

protected:
	Matrix4f m_ProjectionMatrix = Matrix4f(1.0f);
	float m_Exposure = 0.8f;

	Uint32 m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}
