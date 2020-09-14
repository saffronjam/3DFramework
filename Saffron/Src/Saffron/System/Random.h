#pragma once

#include "Saffron/SaffronPCH.h"
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
		return static_cast<T>(m_sUniformDistribution(m_sEngine));
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

	template<typename T = float>
	static T Real(T lower = static_cast<T>(0), T upper = static_cast<T>(1))
	{
		static std::random_device rd;
		static std::mt19937 e(rd());
		std::uniform_real_distribution<T> dis(lower, upper);
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

	template<typename T>
	static glm::vec<3, T> Vec3(const glm::vec<3, T> &low, const glm::vec<3, T> &high)
	{
		return Random::Vec3(low.x, low.y, low.z, high.x, high.y, high.z);
	}
	template<typename T>
	static glm::vec<3, T> Vec3(T lowX, T lowY, T lowZ, T highX, T highY, T highZ)
	{
		float x = Random::Real<T>(lowX, highX);
		float y = Random::Real<T>(lowY, highY);
		float z = Random::Real<T>(lowZ, highZ);
		return { x,y,z };
	}

private:
	T m_Lower;
	T m_Upper;

	static std::random_device m_sRandomDevice;
	static std::mt19937 m_sEngine;
	static std::uniform_int_distribution<T> m_sUniformDistribution;
};
}