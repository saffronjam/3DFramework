#pragma once

#include <format>
#include <limits>

#include <json/json.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "glm/gtx/euler_angles.hpp"

#include "Saffron/Core/TypeDefs.h"

#undef near
#undef far

namespace Se
{
template <uint size>
using Vector = glm::vec<size, float>;
using Vector2 = Vector<2>;
using Vector4 = Vector<4>;

class Quaternion : public glm::quat
{
public:
	using glm::quat::quat;

	Quaternion(const glm::quat& quat) :
		qua(quat)
	{
	}

	static auto CreateFromYawPitchRoll(float yaw, float pitch, float roll) -> Quaternion
	{
		return glm::quat({yaw, pitch, roll});
	}
};

class Vector3 : public glm::vec3
{
public:
	using glm::vec3::vec3;

	Vector3(const glm::vec3& vec) :
		vec(vec)
	{
	}


	auto Normalized() const -> Vector3
	{
		return Vector3(glm::normalize(*this));
	}

	auto Cross(const Vector3 other) const -> Vector3
	{
		return glm::cross(*this, other);
	}
};

class Matrix : public glm::mat4
{
public:
	using glm::mat4::mat;

	Matrix(const glm::mat4& mat) :
		mat(mat)
	{
	}

	auto Decompose() const -> std::tuple<Vector3, Quaternion, Vector3>
	{
		Vector3 scale, translation;
		Quaternion orientation;
		Decompose(scale, orientation, translation);
		return std::make_tuple(scale, orientation, translation);
	}

	auto Decompose(Vector3& scale, Quaternion& orientation, Vector3& translation) const -> bool
	{
		Vector3 sqew_notUsed;
		Vector4 perspective_notUsed;
		return glm::decompose(*this, scale, orientation, translation, sqew_notUsed, perspective_notUsed);
	}

	static auto CreateFromQuaternion(const Quaternion& quaternion) -> Matrix
	{
		return glm::toMat4(quaternion);
	}

	static auto CreateLookAt(const Vector3& position, const Vector3& direction, const Vector3& up) -> Matrix
	{
		return glm::lookAt(position, position + direction, up);
	}

	static auto CreatePerspectiveFieldOfView(float fov, float width, float height, float near, float far) -> Matrix
	{
		return glm::perspectiveFov(fov, width, height, near, far);
	}

	static auto CreateFromRollPitchYaw(float roll, float pitch, float yaw) -> Matrix
	{
		return glm::eulerAngleYXZ(roll, pitch, yaw);
	}

	static auto CreateTranslation(const Vector3& vector) -> Matrix
	{
		return glm::translate(glm::mat4(1), vector);
	}

	static auto CreateTranslation(float x, float y, float z) -> Matrix
	{
		return CreateTranslation({x, y, z});
	}

	static auto CreateScale(float scale) -> Matrix
	{
		return CreateScale({scale, scale, scale});
	}

	static auto CreateScale(float x, float y, float z) -> Matrix
	{
		return CreateScale({x, y, z});
	}

	static auto CreateScale(const Vector3& scale) -> Matrix
	{
		return glm::scale(glm::mat4(1.0f), scale);
	}

	static const Matrix Identity;
};


class Color
{
public:
	float r, g, b, a;
};

struct Colors
{
	static const Color Transparent;
	static const Color Black;
	static const Color White;
	static const Color Red;
};

namespace Math
{
constexpr float Pi = static_cast<float>(3.141592653589793238462643383279);

static float ToDegrees(float radians)
{
	return glm::radians(radians);
}

static float ToRadians(float degrees)
{
	return glm::degrees(degrees);
}

template <int size>
static Vector3 ToDegrees(const Vector<size>& radians)
{
	return glm::radians(radians);
}

template <int size>
static Vector3 ToRadians(const Vector<size>& degrees)
{
	return glm::degrees(degrees);
}

static Vector3 ToEulerAngles(const Quaternion& orientation)
{
	return glm::eulerAngles(orientation);
}
}
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
	auto format(Se::Matrix matrix, std::format_context& ctx)
	{
		auto* m = glm::value_ptr(matrix);
		return formatter<string>::format(std::format(
			                                 "[{:.2f}, {:.2f}, {:.2f}, {:.2f} | {:.2f}, {:.2f}, {:.2f}, {:.2f} | {:.2f}, {:.2f}, {:.2f}, {:.2f} | {:.2f}, {:.2f}, {:.2f}, {:.2f}]",
			                                 m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11],
			                                 m[12], m[13], m[14], m[15]), ctx);
	}
};
