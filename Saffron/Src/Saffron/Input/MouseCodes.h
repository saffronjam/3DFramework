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

#define SE_BUTTON_0      ::Se::ButtonCode::0
#define SE_BUTTON_1      ::Se::ButtonCode::1
#define SE_BUTTON_2      ::Se::ButtonCode::2
#define SE_BUTTON_3      ::Se::ButtonCode::3
#define SE_BUTTON_4      ::Se::ButtonCode::4
#define SE_BUTTON_5      ::Se::ButtonCode::5
#define SE_BUTTON_6      ::Se::ButtonCode::6
#define SE_BUTTON_7      ::Se::ButtonCode::7
#define SE_BUTTON_LAST   ::Se::ButtonCode::Last
#define SE_BUTTON_LEFT   ::Se::ButtonCode::Left
#define SE_BUTTON_RIGHT  ::Se::ButtonCode::Right
#define SE_BUTTON_MIDDLE ::Se::ButtonCode::Middle
