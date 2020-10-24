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

namespace Se
{
class Math
{
public:
	static constexpr float PI = 3.141592653589793238462643383279f;
	static constexpr double PI_D = static_cast<double>(PI);

	static constexpr float E = 2.71828182845904523536f;
	static constexpr double E_D = static_cast<double>(E);


};

constexpr glm::precision DefaultVectorPrecision = glm::defaultp;
constexpr glm::precision DefaultMatrixPrecision = glm::defaultp;

template<glm::length_t Dimension, typename NumberType, glm::precision Precision = DefaultVectorPrecision>
using Vector = glm::vec<Dimension, NumberType, Precision>;
template<typename NumberType>
using Vector2 = Vector<2, NumberType, DefaultVectorPrecision>;
template<typename NumberType>
using Vector3 = Vector<3, NumberType, DefaultVectorPrecision>;
template<typename NumberType>
using Vector4 = Vector<4, NumberType, DefaultVectorPrecision>;

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;

using Vector2d = Vector2<double>;
using Vector3d = Vector3<double>;
using Vector4d = Vector4<double>;

using Vector2i = Vector2<Int32>;
using Vector3i = Vector3<Int32>;
using Vector4i = Vector4<Int32>;

using Vector2u = Vector2<Uint32>;
using Vector3u = Vector3<Uint32>;
using Vector4u = Vector4<Uint32>;

using Vector2b = Vector2<bool>;
using Vector3b = Vector3<bool>;
using Vector4b = Vector4<bool>;

template<glm::length_t DimensionX, glm::length_t DimensionY, typename NumberType, glm::precision Precision = DefaultMatrixPrecision>
using Matrix = glm::mat<DimensionX, DimensionY, NumberType, Precision>;
template<typename NumberType>
using Matrix2 = Matrix<2, 2, NumberType, DefaultMatrixPrecision>;
template<typename NumberType>
using Matrix3 = Matrix<3, 3, NumberType, DefaultMatrixPrecision>;
template<typename NumberType>
using Matrix4 = Matrix<4, 4, NumberType, DefaultMatrixPrecision>;

using Matrix2f = Matrix2<float>;
using Matrix3f = Matrix3<float>;
using Matrix4f = Matrix4<float>;

using Matrix2d = Matrix2<double>;
using Matrix3d = Matrix3<double>;
using Matrix4d = Matrix4<double>;

using Matrix2i = Matrix2<Int32>;
using Matrix3i = Matrix3<Int32>;
using Matrix4i = Matrix4<Int32>;

using Matrix2u = Matrix2<Uint32>;
using Matrix3u = Matrix3<Uint32>;
using Matrix4u = Matrix4<Uint32>;

using Matrix2b = Matrix2<bool>;
using Matrix3b = Matrix3<bool>;
using Matrix4b = Matrix4<bool>;


using Quaternion = glm::quat;

}
