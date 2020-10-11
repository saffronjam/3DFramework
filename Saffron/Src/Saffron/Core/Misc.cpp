#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"

namespace Se
{

Misc::TransformDecomposition Misc::GetTransformDecomposition(const glm::mat4 &transform)
{
	glm::vec3 scale, translation, skew;
	glm::vec4 perspective;
	glm::quat orientation;
	glm::decompose(transform, scale, orientation, translation, skew, perspective);

	return { translation, orientation, scale };
}

}
