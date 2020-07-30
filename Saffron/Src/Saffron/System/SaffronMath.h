#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Saffron/Config.h"

namespace Se
{
class Math
{
public:
	float PI = 3.141592653589793238462643383279f;
	double PI_D = static_cast<double>(PI);

	float E = 2.71828182845904523536f;
	double E_D = static_cast<double>(E);
};
}
