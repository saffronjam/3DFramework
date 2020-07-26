#pragma once

#include "Saffron/SaffronPCH.h"

namespace Saffron
{
enum class ButtonCode
{
	Num1 = 0,
	Num2 = 1,
	Num3 = 2,
	Num4 = 3,
	Num5 = 4,
	Num6 = 5,
	Num7 = 6,
	Num8 = 7,
	Last = Num8,
	Left = Num1,
	Right = Num2,
	Middle = Num3,
	Undefined = 100
};

inline std::ostream &operator<<(std::ostream &os, ButtonCode buttonCode)
{
	os << static_cast<int>(buttonCode);
	return os;
}
}

#define SE_MOUSE_BUTTON_0      ::Saffron::MouseCode::Button0
#define SE_MOUSE_BUTTON_1      ::Saffron::MouseCode::Button1
#define SE_MOUSE_BUTTON_2      ::Saffron::MouseCode::Button2
#define SE_MOUSE_BUTTON_3      ::Saffron::MouseCode::Button3
#define SE_MOUSE_BUTTON_4      ::Saffron::MouseCode::Button4
#define SE_MOUSE_BUTTON_5      ::Saffron::MouseCode::Button5
#define SE_MOUSE_BUTTON_6      ::Saffron::MouseCode::Button6
#define SE_MOUSE_BUTTON_7      ::Saffron::MouseCode::Button7
#define SE_MOUSE_BUTTON_LAST   ::Saffron::MouseCode::ButtonLast
#define SE_MOUSE_BUTTON_LEFT   ::Saffron::MouseCode::ButtonLeft
#define SE_MOUSE_BUTTON_RIGHT  ::Saffron::MouseCode::ButtonRight
#define SE_MOUSE_BUTTON_MIDDLE ::Saffron::MouseCode::ButtonMiddle