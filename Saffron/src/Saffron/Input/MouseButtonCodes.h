#pragma once

#include "SaffronPCH.h"

namespace Se
{
enum class MouseButtonCode
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

inline std::ostream &operator<<(std::ostream &os, MouseButtonCode buttonCode)
{
	os << static_cast<int>(buttonCode);
	return os;
}
}

#define SE_BUTTON_0      ::Se::MouseButtonCode::0
#define SE_BUTTON_1      ::Se::MouseButtonCode::1
#define SE_BUTTON_2      ::Se::MouseButtonCode::2
#define SE_BUTTON_3      ::Se::MouseButtonCode::3
#define SE_BUTTON_4      ::Se::MouseButtonCode::4
#define SE_BUTTON_5      ::Se::MouseButtonCode::5
#define SE_BUTTON_6      ::Se::MouseButtonCode::6
#define SE_BUTTON_7      ::Se::MouseButtonCode::7
#define SE_BUTTON_LAST   ::Se::MouseButtonCode::Last
#define SE_BUTTON_LEFT   ::Se::MouseButtonCode::Left
#define SE_BUTTON_RIGHT  ::Se::MouseButtonCode::Right
#define SE_BUTTON_MIDDLE ::Se::MouseButtonCode::Middle
