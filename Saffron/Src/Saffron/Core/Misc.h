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
	template<typename T>
	static T Clamped(const T &value, const T &lower, const T &upper)
	{
		if ( value < lower )
		{
			return lower;
		}
		if ( value > upper )
		{
			return upper;
		}
		return value;
	}
	template<typename T>
	static void Clamp(T &value, const T &lower, const T &upper)
	{
		if ( value < lower )
		{
			value = lower;
		}
		if ( value > upper )
		{
			value = upper;
		}
	}
};

}