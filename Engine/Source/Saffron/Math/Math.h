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
using Rect = DirectX::SimpleMath::Rectangle;

class Math
{
public:
	static constexpr float Pi = static_cast<float>(M_PI);

	
};
}
