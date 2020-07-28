#pragma once

#include "Saffron/SaffronPCH.h"

namespace Se
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

#define SE_MOUSE_BUTTON_0      ::Se::ButtonCode::Button0
#define SE_MOUSE_BUTTON_1      ::Se::ButtonCode::Button1
#define SE_MOUSE_BUTTON_2      ::Se::ButtonCode::Button2
#define SE_MOUSE_BUTTON_3      ::Se::ButtonCode::Button3
#define SE_MOUSE_BUTTON_4      ::Se::ButtonCode::Button4
#define SE_MOUSE_BUTTON_5      ::Se::ButtonCode::Button5
#define SE_MOUSE_BUTTON_6      ::Se::ButtonCode::Button6
#define SE_MOUSE_BUTTON_7      ::Se::ButtonCode::Button7
#define SE_MOUSE_BUTTON_LAST   ::Se::ButtonCode::ButtonLast
#define SE_MOUSE_BUTTON_LEFT   ::Se::ButtonCode::ButtonLeft
#define SE_MOUSE_BUTTON_RIGHT  ::Se::ButtonCode::ButtonRight
#define SE_MOUSE_BUTTON_MIDDLE ::Se::ButtonCode::ButtonMiddle
