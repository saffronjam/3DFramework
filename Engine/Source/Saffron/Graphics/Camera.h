#pragma once

#include "Saffron/Base.h"

namespace Se
{
struct CameraData
{
	Matrix View;
	Matrix Projection;
	Vector3 Position;
};

struct ProjectionSpec
{
	float Fov;
	float AspectRatio;
	float Near;
	float Far;
};

class Camera
{
public:
	Camera();
	explicit Camera(const Matrix& projection);
	virtual ~Camera() = default;

	virtual auto Data() const -> CameraData = 0;

	auto Projection() const -> const Matrix&;
	void SetProjection(const struct ProjectionSpec& spec);

	auto ProjectionSpec() const -> const struct ProjectionSpec&;

private:
	Matrix _projection;

	union
	{
		struct ProjectionSpec _projectionSpec;
	};
};
}
