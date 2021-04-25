#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Se
{
constexpr glm::precision DefaultVectorPrecision = glm::defaultp;
constexpr glm::precision DefaultMatrixPrecision = glm::defaultp;

template <glm::length_t Dimension, typename NumberType, glm::precision Precision = DefaultVectorPrecision>
using Vector = glm::vec<Dimension, NumberType, Precision>;

using Vector2 = Vector<2, float, DefaultVectorPrecision>;
using Vector3 = Vector<3, float, DefaultVectorPrecision>;
using Vector4 = Vector<4, float, DefaultVectorPrecision>;

using Vector2i = Vector<2, int, DefaultVectorPrecision>;
using Vector3i = Vector<3, int, DefaultVectorPrecision>;
using Vector4i = Vector<4, int, DefaultVectorPrecision>;

using Vector2u = Vector<2, uint, DefaultVectorPrecision>;
using Vector3u = Vector<3, uint, DefaultVectorPrecision>;
using Vector4u = Vector<4, uint, DefaultVectorPrecision>;

template <glm::length_t DimensionX, glm::length_t DimensionY, typename NumberType, glm::precision Precision =
	          DefaultMatrixPrecision>
using Matrix = glm::mat<DimensionX, DimensionY, NumberType, Precision>;

using Matrix2 = Matrix<2, 2, float, DefaultMatrixPrecision>;
using Matrix3 = Matrix<3, 3, float, DefaultMatrixPrecision>;
using Matrix4 = Matrix<4, 4, float, DefaultMatrixPrecision>;

using Quaternion = glm::quat;

class Math
{
public:
	static constexpr float PI = 3.141592653589793238462643383279f;
	static constexpr double PI_D = static_cast<double>(PI);

	static constexpr float E = 2.71828182845904523536f;
	static constexpr double E_D = static_cast<double>(E);

	static Matrix4 ComposeMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		return translate(translation) * toMat4(rotation) * glm::scale(scale);
	}
};
}
