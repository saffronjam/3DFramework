#include "SaffronPCH.h"

#include <functional>

#include "Saffron/Common/Misc.h"

namespace Se
{
Misc::TransformDecomposition Misc::GetTransformDecomposition(const Matrix& transform)
{
	Vector3 scale, translation, skew;
	Vector4 perspective;
	Quaternion orientation;

	auto& nonConst = const_cast<Matrix&>(transform);
	nonConst.Decompose(scale, orientation, translation);

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
