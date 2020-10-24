#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class Misc
{
public:
	struct TransformDecomposition
	{
		Vector3f Translation;
		Quaternion Rotation;
		Vector3f Scale;
	};

public:
	static TransformDecomposition GetTransformDecomposition(const Matrix4f &transform);
};

}