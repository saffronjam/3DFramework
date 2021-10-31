#pragma once

#include <format>

#include <DirectXTK/SimpleMath.h>

namespace Se
{
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;

using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

using Color = DirectX::SimpleMath::Color;

struct Colors
{
	static const Color Transparent;
	static const Color Black;
	static const Color White;
	static const Color Red;
};

class Math
{
public:
	static constexpr float Pi = static_cast<float>(M_PI);
};
}

template <>
struct std::formatter<Se::Vector2> : std::formatter<std::string>
{
	auto format(Se::Vector2 p, std::format_context& ctx)
	{
		return formatter<string>::format(std::format("[{:.2f}, {:.2f}]", p.x, p.y), ctx);
	}
};

template <>
struct std::formatter<Se::Vector3> : std::formatter<std::string>
{
	auto format(Se::Vector3 p, std::format_context& ctx)
	{
		return formatter<string>::format(std::format("[{:.2f}, {:.2f}, {:.2f}]", p.x, p.y, p.z), ctx);
	}
};

template <>
struct std::formatter<Se::Vector4> : std::formatter<std::string>
{
	auto format(Se::Vector4 p, std::format_context& ctx)
	{
		return formatter<string>::format(std::format("[{:.2f}, {:.2f}, {:.2f}, {:.2f}]", p.x, p.y, p.z, p.w), ctx);
	}
};

template <>
struct std::formatter<Se::Matrix> : std::formatter<std::string>
{
	auto format(Se::Matrix m, std::format_context& ctx)
	{
		return formatter<string>::format(
			std::format(
				"[{:.2f}, {:.2f}, {:.2f}, {:.2f} | {:.2f}, {:.2f}, {:.2f}, {:.2f} | {:.2f}, {:.2f}, {:.2f}, {:.2f} | {:.2f}, {:.2f}, {:.2f}, {:.2f}]",
				m.m[0][0],
				m.m[0][1],
				m.m[0][2],
				m.m[0][3],
				m.m[1][0],
				m.m[1][1],
				m.m[1][2],
				m.m[1][3],
				m.m[2][0],
				m.m[2][1],
				m.m[2][2],
				m.m[2][3],
				m.m[3][0],
				m.m[3][1],
				m.m[3][2],
				m.m[3][3]
			),
			ctx
		);
	}
};
