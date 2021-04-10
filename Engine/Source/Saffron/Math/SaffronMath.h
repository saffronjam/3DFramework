#pragma once

#include <DirectXTK/SimpleMath.h>

namespace Se
{
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;

using Matrix = DirectX::SimpleMath::Matrix;

using Quaternion = DirectX::SimpleMath::Quaternion;

using Color = DirectX::SimpleMath::Color;

class Math
{
public:
	static constexpr float PI = DirectX::XM_PI;
	static constexpr float PI2 = DirectX::XM_2PI;

	static constexpr float E = 2.71828182845904523536f;

	static Matrix ComposeMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		return Matrix::CreateTranslation(translation) * Matrix::CreateFromQuaternion(rotation) *
			Matrix::CreateScale(scale);
	}

	template <typename T>
	static constexpr T Gauss(T x, T sigma) noexcept
	{
		const auto ss = std::pow(sigma, 2.0);
		return (static_cast<T>(1.0) / std::sqrt(static_cast<T>(2.0) * static_cast<T>(PI) * ss)) * std::exp(
			-std::pow(x, 2.0) / (static_cast<T>(2.0) * ss));
	}
};
}
