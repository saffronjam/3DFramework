#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class Misc
{
public:
	struct TransformDecomposition
	{
		glm::vec3 Translation;
		glm::quat Rotation;
		glm::vec3 Scale;
	};

public:
	static TransformDecomposition GetTransformDecomposition(const glm::mat4 &transform);
};

}