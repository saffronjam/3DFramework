#pragma once

#include <random>

#include "Saffron/Math/SaffronMath.h"

namespace Se
{
template <typename T>
using UniformRealDistribution = std::uniform_real_distribution<T>;
template <typename T>
using UniformIntDistribution = std::uniform_int_distribution<T>;


class Random
{
public:
	using Device = std::random_device;
	using Engine = std::mt19937;

public:
	template <typename IntegralType>
	static int Integer(IntegralType lower = static_cast<IntegralType>(0),
	                   IntegralType upper = static_cast<IntegralType>(100))
	{
		static_assert(std::is_integral<IntegralType>::value, "IntegralType must be integral type");
		static Device s_Device;
		static Engine s_Engine(s_Device());
		UniformIntDistribution<IntegralType> distribution(lower, upper);
		return distribution(s_Engine);
	}

	template <typename RealType = float>
	static RealType Real(RealType lower = static_cast<RealType>(0), RealType upper = static_cast<RealType>(1))
	{
		static_assert(std::is_floating_point<RealType>::value, "RealType must be floating point type");
		static Device s_Device;
		static Engine s_Engine(s_Device());
		UniformRealDistribution<RealType> distribution(lower, upper);
		return distribution(s_Engine);
	}

	static Vector3f Vector3()
	{
		return Vector3f{Real(0.0f, 1.0f), Real(0.0f, 1.0f), Real(0.0f, 1.0f)};
	}
};

template <typename T>
class RandomGenerator : Random
{
public:
	explicit RandomGenerator(T lower = static_cast<T>(0), T upper = static_cast<T>(100)) :
		_lower(lower),
		_upper(upper)
	{
	}

	T Generate()
	{
		static Device s_RandomDevice;
		static Engine s_Engine(s_RandomDevice());

		return _lower + static_cast<T>(GetUniformDistribution()(s_Engine)) * (_upper - _lower);
	}

	void SetLower(T lower) { _lower = lower; }

	void SetUpper(T upper) { _upper = upper; }

private:
	auto& GetUniformDistribution()
	{
		static_assert(std::is_arithmetic<T>::value);
		if constexpr (std::is_integral<T>::value)
		{
			static UniformIntDistribution<T> s_Distribution;
			return s_Distribution;
		}
		else
		{
			static UniformRealDistribution<T> s_Distribution;
			return s_Distribution;
		}
	}

	T _lower, _upper;
};
}
