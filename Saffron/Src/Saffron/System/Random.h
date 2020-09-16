#pragma once

#include <random>

#include "Saffron/System/SaffronMath.h"

namespace Se
{

template<typename T>
class Random
{
public:
	explicit Random(T lower = static_cast<T>(0), T upper = static_cast<T>(100))
		:
		m_Lower(lower),
		m_Upper(upper)
	{
	};

	T Generate()
	{
		return static_cast<T>(GetUniformDistribution()(GetEngine()));
	}

	void SetLower(T lower) { m_Lower = lower; };
	void SetUpper(T upper) { m_Upper = upper; };

	static int Integer(int lower = 0, int upper = 100)
	{
		// TODO: Remove
		/*static std::random_device sRandomDevice;
		static std::mt19937 sEngine(sRandomDevice());
		const std::uniform_real_distribution<int> sUniformDistribution(lower, upper);
		return static_cast<int>(sUniformDistribution(sEngine));*/

		return Real<int>(lower, upper);
	}

	template<typename U = float>
	static U Real(U lower = static_cast<U>(0), U upper = static_cast<U>(1))
	{
		static std::random_device rd;
		static std::mt19937 e(rd());
		std::uniform_real_distribution<U> dis(lower, upper);
		return dis(e);
	}

	//    static sf::Color Color(bool randomizeAlpha = false)
	//    {
	//        sf::Uint8 r = Random::Integer(0, 255);
	//        sf::Uint8 g = Random::Integer(0, 255);
	//        sf::Uint8 b = Random::Integer(0, 255);
	//        sf::Uint8 a = 255;
	//        if (randomizeAlpha)
	//            a = Random::Integer(0, 255);
	//        return sf::Color(r, g, b, a);
	//    }

	template<typename U>
	static glm::vec<3, U> Vec3(const glm::vec<3, U> &low, const glm::vec<3, U> &high)
	{
		return Random::Vec3(low.x, low.y, low.z, high.x, high.y, high.z);
	}
	template<typename U>
	static glm::vec<3, U> Vec3(U lowX, U lowY, U lowZ, U highX, U highY, U highZ)
	{
		float x = Random::Real<U>(lowX, highX);
		float y = Random::Real<U>(lowY, highY);
		float z = Random::Real<U>(lowZ, highZ);
		return { x,y,z };
	}
private:
	auto &GetUniformDistribution() const
	{
		static std::uniform_int_distribution<T> sUniformDistribution;
		return sUniformDistribution;
	}

	auto &GetDevice() const
	{
		static std::random_device sRandomDevice;
		return sRandomDevice;
	}

	auto &GetEngine() const
	{
		static std::mt19937 sEngine(GetDevice()());
		return sEngine;
	}

private:
	T m_Lower;
	T m_Upper;
};
}