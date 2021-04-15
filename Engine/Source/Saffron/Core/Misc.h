#pragma once

#include "Saffron/Math/SaffronMath.h"

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

	template<typename T>
	static void LowClamp(T &value, const T &lower)
	{
		if ( value < lower )
		{
			value = lower;
		}
	}

	template<typename T>
	static void HighClamp(T &value, const T &upper)
	{
		if ( value > upper )
		{
			value = upper;
		}
	}


	template<typename t_DestVector3, typename t_SourceVector3>
	constexpr static const t_DestVector3 &ConvertTo(const t_SourceVector3 &source)
	{
		return *reinterpret_cast<const t_DestVector3 *>(&source);
	}
};

}