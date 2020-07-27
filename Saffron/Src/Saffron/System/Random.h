#pragma once

#include <utility>
#include <random>

#include "SeMath.h"

namespace Se
{
class Random
{
public:
	Random(int const &lower = 0, int const &upper = 10)
		: m_lower(lower),
		m_upper(upper)
	{
	};

	int Gen() { return (double)((rand() % ((int)m_upper - (int)m_lower)) + (int)m_lower); }

	void SetLowerBound(int lowerBound_IN) { m_lower = lowerBound_IN; };
	void SetUpperBound(int upperBound_IN) { m_upper = upperBound_IN; };

	static int Integer(int low = 0, int high = 100)
	{
		static std::random_device rd;
		static std::mt19937 e(rd());
		std::uniform_real_distribution<float> dis(static_cast<float>(low), static_cast<float>(high));
		return static_cast<int>(dis(e));
	}

	template<typename T = float>
	static T Real(T low = (T)0, T high = (T)1)
	{
		static std::random_device rd;
		static std::mt19937 e(rd());
		std::uniform_real_distribution<T> dis(low, high);
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
	int m_lower;
	int m_upper;
};
}