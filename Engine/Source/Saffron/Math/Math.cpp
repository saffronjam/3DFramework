#include "SaffronPCH.h"


#include "Saffron/Math/Math.h"


namespace Se
{
const Color Colors::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
const Color Colors::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Colors::White(1.0f, 1.0f, 1.0f, 1.0f);
const Color Colors::Red(1.0f, 0.0f, 0.0f, 1.0f);

Vector3 Math::ToEulerAngles(Quaternion q)
{
	std::swap(q.x, q.y);

	Vector3 angles;

	// yaw (x-axis rotation)
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.x = std::atan2(siny_cosp, cosy_cosp);

	// pitch (y-axis rotation)
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1.0f) angles.y = std::copysign(Pi / 2.0f, sinp); // use 90 degrees if out of range
	else angles.y = std::asin(sinp);

	// roll (z-axis rotation)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	angles.z = std::atan2(sinr_cosp, cosr_cosp);

	std::swap(angles.x, angles.z);

	return angles;
}
}
