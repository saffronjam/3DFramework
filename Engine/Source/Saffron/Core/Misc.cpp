#include "SaffronPCH.h"

#include <functional>

#include "Saffron/Core/Misc.h"

namespace Se
{
Misc::TransformDecomposition Misc::GetTransformDecomposition(const Matrix4& transform)
{
	Vector3 scale, translation, skew;
	Vector4 perspective;
	glm::quat orientation;
	decompose(transform, scale, orientation, translation, skew, perspective);

	return {translation, orientation, scale};
}

size_t Misc::HashFilepath(const Filepath& filepath)
{
	return std::hash<String>{}(filepath.string());
}

size_t Misc::HashString(const String& string)
{
	return std::hash<String>{}(string);
}
}
