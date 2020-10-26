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
	static size_t HashFilepath(const Filepath &filepath);
	static size_t HashString(const String &string);
};

}