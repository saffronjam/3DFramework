#pragma once

#include <random>

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
	static auto Integer(IntegralType lower = static_cast<IntegralType>(0),
	                    IntegralType upper = static_cast<IntegralType>(100)) -> int
	{
		static_assert(std::is_integral<IntegralType>::value, "IntegralType must be integral type");
		static Device s_Device;
		static Engine s_Engine(s_Device());
		UniformIntDistribution<IntegralType> distribution(lower, upper);
		return distribution(s_Engine);
	}

	template <typename RealType = float>
	static auto Real(RealType lower = static_cast<RealType>(0), RealType upper = static_cast<RealType>(1)) -> RealType
	{
		static_assert(std::is_floating_point<RealType>::value, "RealType must be floating point type");
		static Device s_Device;
		static Engine s_Engine(s_Device());
		UniformRealDistribution<RealType> distribution(lower, upper);
		return distribution(s_Engine);
	}

	/*template <typename NumberType>
	static auto Vec2(const sf::Vector2<NumberType>& low, const sf::Vector2<NumberType>& high) -> sf::Vector2<NumberType>
	{
		return Vec2(low.x, low.y, high.x, high.y);
	}*/

	/*template <typename NumberType>
	static auto Vec2(NumberType lowX, NumberType lowY, NumberType highX, NumberType highY) -> sf::Vector2<NumberType>
	{
		static_assert(std::is_arithmetic<NumberType>::value, "NumberType must be arithmetic type");
		if constexpr (std::is_integral<NumberType>::value)
		{
			float x = Integer<NumberType>(lowX, highX);
			float y = Integer<NumberType>(lowY, highY);
			return {x, y};
		}
		else
		{
			float x = Real<NumberType>(lowX, highX);
			float y = Real<NumberType>(lowY, highY);
			return {x, y};
		}
	}*/

	/*template <typename NumberType>
	static auto Vec3(const sf::Vector3<NumberType>& low, const sf::Vector3<NumberType>& high) -> sf::Vector3<NumberType>
	{
		return Vec3(low.x, low.y, low.z, high.x, high.y, high.z);
	}*/

	/*template <typename NumberType>
	static auto Vec3(NumberType lowX, NumberType lowY, NumberType lowZ, NumberType highX, NumberType highY,
	                 NumberType highZ) -> sf::Vector3<NumberType>
	{
		static_assert(std::is_arithmetic<NumberType>::value, "NumberType must be arithmetic type");
		if constexpr (std::is_integral<NumberType>::value)
		{
			float x = Integer<NumberType>(lowX, highX);
			float y = Integer<NumberType>(lowY, highY);
			float z = Integer<NumberType>(lowZ, highZ);
			return {x, y, z};
		}
		else
		{
			float x = Real<NumberType>(lowX, highX);
			float y = Real<NumberType>(lowY, highY);
			float z = Real<NumberType>(lowZ, highZ);
			return {x, y, z};
		}
	}*/

	/*template<typename NumberType>
	static auto Vec4(const sf::Vector4<NumberType>& low, const sf::Vector4<NumberType>& high) -> sf::Vector4<NumberType>
	{
		return Vec4(low.x, low.y, low.z, low.w, high.x, high.y, high.z, high.w);
	}*/

	/*template<typename NumberType>
	static auto Vec4(NumberType lowX, NumberType lowY, NumberType lowZ, NumberType lowW, NumberType highX,
	                 NumberType highY, NumberType highZ, NumberType highW) -> sf::Vector4<NumberType>
	{
		static_assert(std::is_arithmetic<NumberType>::value, "NumberType must be arithmetic type");
		if constexpr ( std::is_integral<NumberType>::value )
		{
			const auto x = static_cast<NumberType>(Integer<NumberType>(lowX, highX));
			const auto y = static_cast<NumberType>(Integer<NumberType>(lowY, highY));
			const auto z = static_cast<NumberType>(Integer<NumberType>(lowZ, highZ));
			const auto w = static_cast<NumberType>(Integer<NumberType>(lowW, highW));
			return { x, y, z, w };
		}
		else
		{
			const auto x = static_cast<NumberType>(Real<NumberType>(lowX, highX));
			const auto y = static_cast<NumberType>(Real<NumberType>(lowY, highY));
			const auto z = static_cast<NumberType>(Real<NumberType>(lowZ, highZ));
			const auto w = static_cast<NumberType>(Real<NumberType>(lowW, highW));
			return { x, y, z, w };
		}
	}*/

	/*static auto Color(bool randomizeAlpha = false) -> sf::Color
	{
		const auto bVec = VecUtils::ConvertTo<sf::Vector4<uchar>>(Vec4(0, 0, 0, randomizeAlpha ? 0 : 255, 255, 255, 255, 255));
		return sf::Color(bVec.x, bVec.y, bVec.z, bVec.w);
	}*/
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
		static Device s_RandomDevice;
		static Engine s_Engine(s_RandomDevice());

		return _lower + static_cast<T>(GetUniformDistribution()(s_Engine)) * (_upper - _lower);
	}

	void SetLower(T lower) { _lower = lower; }

	void SetUpper(T upper) { _upper = upper; }

private:
	auto GetUniformDistribution() -> auto&
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
