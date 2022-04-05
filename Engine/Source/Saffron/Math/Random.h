#pragma once

#include <random>

#include "Saffron/Math/Math.h"

namespace Se
{
class Random
{
public:
	using Device = std::random_device;
	using Engine = std::mt19937;

public:
	static auto Integer(int lower = 0, int upper = 100) -> int
	{
		static Device s_Device;
		static Engine s_Engine(s_Device());
		const std::uniform_int_distribution distribution(lower, upper);
		return distribution(s_Engine);
	}

	static auto Real(float lower = 0.0f, float upper = 1.0f) -> float
	{
		static Device s_Device;
		static Engine s_Engine(s_Device());
		const std::uniform_real_distribution distribution(lower, upper);
		return distribution(s_Engine);
	}

	static auto Vec2(const Vector2& low, const Vector2& high) -> Vector2
	{
		return {Real(low.x, high.x), Real(low.y, high.y)};
	}

	static auto Vec2(float lowX, float lowY, float highX, float highY) -> Vector2
	{
		return {Real(lowX, highX), Real(lowY, highY)};
	}

	static auto Vec3(const Vector3& low, const Vector3& high) -> Vector3
	{
		return {Real(low.x, high.x), Real(low.y, high.y), Real(low.z, high.z)};
	}

	static auto Vec3(
		float lowX = 0.0f,
		float lowY = 0.0f,
		float lowZ = 0.0f,
		float highX = 1.0f,
		float highY = 1.0f,
		float highZ = 1.0f
	) -> Vector3
	{
		return {Real(lowX, highX), Real(lowY, highY), Real(lowZ, highZ)};
	}

	static auto Vec4(
		const Vector4& low = {0.0f, 0.0f, 0.0f, 0.0f},
		const Vector4& high = {1.0f, 1.0f, 1.0f, 1.0f}
	) -> Vector4
	{
		return {Real(low.x, high.x), Real(low.y, high.y), Real(low.z, high.z), Real(low.w, high.w)};
	}

	static auto Vec4(
		float lowX = 0.0f,
		float lowY = 0.0f,
		float lowZ = 0.0f,
		float lowW = 0.0f,
		float highX = 1.0f,
		float highY = 1.0f,
		float highZ = 1.0f,
		float highW = 0.0f
	) -> Vector4
	{
		return {Real(lowX, highX), Real(lowY, highY), Real(lowZ, highZ), Real(lowW, highW)};
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

	auto Generate() -> T
	{
		static Device _randomDevice;
		static Engine _engine(_randomDevice());

		return _lower + static_cast<T>(UniformDistribution()(_engine)) * (_upper - _lower);
	}

	void SetLower(T lower) { _lower = lower; }

	void SetUpper(T upper) { _upper = upper; }

private:
	auto UniformDistribution() -> auto&
	{
		if constexpr (std::is_integral_v<T>)
		{
			static std::uniform_int_distribution _distribution;
			return _distribution;
		}
		else
		{
			static std::uniform_real_distribution<T> _distribution;
			return _distribution;
		}
	}

	T _lower, _upper;
};
}
