#pragma once

#include "Saffron/SaffronPCH.h"

namespace Se
{
enum class KeyCode
{
	Space = 32,
	Apostrophe = 39,
	Comma = 44,
	Minus = 45,
	Period = 46,
	Slash = 47,
	Num0 = 48,
	Num1 = 49,
	Num2 = 50,
	Num3 = 51,
	Num4 = 52,
	Num5 = 53,
	Num6 = 54,
	Num7 = 55,
	Num8 = 56,
	Num9 = 57,
	Semicolon = 59, /* , */
	Equal = 61, /* = */
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LeftBracket = 91, /* [ */
	Backslash = 92, /* \ */
	RightBracket = 93, /* ] */
	GraveAccent = 96, /* ` */
	World1 = 161, /* non-US #1 */
	World2 = 162, /* non-US #2 */
	Escape = 256,
	Enter = 257,
	Tab = 258,
	Backspace = 259,
	Insert = 260,
	Delete = 261,
	Right = 262,
	Left = 263,
	Down = 264,
	Up = 265,
	PageUp = 266,
	PageDown = 267,
	Home = 268,
	End = 269,
	CapsLock = 280,
	ScrollLock = 281,
	NumLock = 282,
	PrintScreen = 283,
	Pause = 284,
	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,
	KP0 = 320,
	KP1 = 321,
	KP2 = 322,
	KP3 = 323,
	KP4 = 324,
	KP5 = 325,
	KP6 = 326,
	KP7 = 327,
	KP8 = 328,
	KP9 = 329,
	KPDecimal = 330,
	KPDivide = 331,
	KPMultiply = 332,
	KPSubtract = 333,
	KPAdd = 334,
	KPEnter = 335,
	KPEqual = 336,
	LeftShift = 340,
	LeftControl = 341,
	LeftAlt = 342,
	LeftSuper = 343,
	RightShift = 344,
	RightControl = 345,
	RightAlt = 346,
	RightSuper = 347,
	Menu = 348,
	Undefined = 1000
};

inline std::ostream &operator<<(std::ostream &os, KeyCode keyCode)
{
	os << static_cast<int>(keyCode);
	return os;
}
}


#define SE_KEY_SPACE           ::Se::Key::Space
#define SE_KEY_APOSTROPHE      ::Se::Key::Apostrophe    /* ' */
#define SE_KEY_COMMA           ::Se::Key::Comma         /* , */
#define SE_KEY_MINUS           ::Se::Key::Minus         /* - */
#define SE_KEY_PERIOD          ::Se::Key::Period        /* . */
#define SE_KEY_SLASH           ::Se::Key::Slash         /* / */
#define SE_KEY_0               ::Se::Key::D0
#define SE_KEY_1               ::Se::Key::D1
#define SE_KEY_2               ::Se::Key::D2
#define SE_KEY_3               ::Se::Key::D3
#define SE_KEY_4               ::Se::Key::D4
#define SE_KEY_5               ::Se::Key::D5
#define SE_KEY_6               ::Se::Key::D6
#define SE_KEY_7               ::Se::Key::D7
#define SE_KEY_8               ::Se::Key::D8
#define SE_KEY_9               ::Se::Key::D9
#define SE_KEY_SEMICOLON       ::Se::Key::Semicolon     /* ; */
#define SE_KEY_EQUAL           ::Se::Key::Equal         /* = */
#define SE_KEY_A               ::Se::Key::A
#define SE_KEY_B               ::Se::Key::B
#define SE_KEY_C               ::Se::Key::C
#define SE_KEY_D               ::Se::Key::D
#define SE_KEY_E               ::Se::Key::E
#define SE_KEY_F               ::Se::Key::F
#define SE_KEY_G               ::Se::Key::G
#define SE_KEY_H               ::Se::Key::H
#define SE_KEY_I               ::Se::Key::I
#define SE_KEY_J               ::Se::Key::J
#define SE_KEY_K               ::Se::Key::K
#define SE_KEY_L               ::Se::Key::L
#define SE_KEY_M               ::Se::Key::M
#define SE_KEY_N               ::Se::Key::N
#define SE_KEY_O               ::Se::Key::O
#define SE_KEY_P               ::Se::Key::P
#define SE_KEY_Q               ::Se::Key::Q
#define SE_KEY_R               ::Se::Key::R
#define SE_KEY_S               ::Se::Key::S
#define SE_KEY_T               ::Se::Key::T
#define SE_KEY_U               ::Se::Key::U
#define SE_KEY_V               ::Se::Key::V
#define SE_KEY_W               ::Se::Key::W
#define SE_KEY_X               ::Se::Key::X
#define SE_KEY_Y               ::Se::Key::Y
#define SE_KEY_Z               ::Se::Key::Z
#define SE_KEY_LEFT_BRACKET    ::Se::Key::LeftBracket   /* [ */
#define SE_KEY_BACKSLASH       ::Se::Key::Backslash     /* \ */
#define SE_KEY_RIGHT_BRACKET   ::Se::Key::RightBracket  /* ] */
#define SE_KEY_GRAVE_ACCENT    ::Se::Key::GraveAccent   /* ` */
#define SE_KEY_WORLD_1         ::Se::Key::World1        /* non-US #1 */
#define SE_KEY_WORLD_2         ::Se::Key::World2        /* non-US #2 */

/* Function keys */
#define SE_KEY_ESCAPE          ::Se::Key::Escape
#define SE_KEY_ENTER           ::Se::Key::Enter
#define SE_KEY_TAB             ::Se::Key::Tab
#define SE_KEY_BACKSPACE       ::Se::Key::Backspace
#define SE_KEY_INSERT          ::Se::Key::Insert
#define SE_KEY_DELETE          ::Se::Key::Delete
#define SE_KEY_RIGHT           ::Se::Key::Right
#define SE_KEY_LEFT            ::Se::Key::Left
#define SE_KEY_DOWN            ::Se::Key::Down
#define SE_KEY_UP              ::Se::Key::Up
#define SE_KEY_PAGE_UP         ::Se::Key::PageUp
#define SE_KEY_PAGE_DOWN       ::Se::Key::PageDown
#define SE_KEY_HOME            ::Se::Key::Home
#define SE_KEY_END             ::Se::Key::End
#define SE_KEY_CAPS_LOCK       ::Se::Key::CapsLock
#define SE_KEY_SCROLL_LOCK     ::Se::Key::ScrollLock
#define SE_KEY_NUM_LOCK        ::Se::Key::NumLock
#define SE_KEY_PRINT_SCREEN    ::Se::Key::PrintScreen
#define SE_KEY_PAUSE           ::Se::Key::Pause
#define SE_KEY_F1              ::Se::Key::F1
#define SE_KEY_F2              ::Se::Key::F2
#define SE_KEY_F3              ::Se::Key::F3
#define SE_KEY_F4              ::Se::Key::F4
#define SE_KEY_F5              ::Se::Key::F5
#define SE_KEY_F6              ::Se::Key::F6
#define SE_KEY_F7              ::Se::Key::F7
#define SE_KEY_F8              ::Se::Key::F8
#define SE_KEY_F9              ::Se::Key::F9
#define SE_KEY_F10             ::Se::Key::F10
#define SE_KEY_F11             ::Se::Key::F11
#define SE_KEY_F12             ::Se::Key::F12
#define SE_KEY_F13             ::Se::Key::F13
#define SE_KEY_F14             ::Se::Key::F14
#define SE_KEY_F15             ::Se::Key::F15
#define SE_KEY_F16             ::Se::Key::F16
#define SE_KEY_F17             ::Se::Key::F17
#define SE_KEY_F18             ::Se::Key::F18
#define SE_KEY_F19             ::Se::Key::F19
#define SE_KEY_F20             ::Se::Key::F20
#define SE_KEY_F21             ::Se::Key::F21
#define SE_KEY_F22             ::Se::Key::F22
#define SE_KEY_F23             ::Se::Key::F23
#define SE_KEY_F24             ::Se::Key::F24
#define SE_KEY_F25             ::Se::Key::F25

/* Keypad */
#define SE_KEY_KP_0            ::Se::Key::KP0
#define SE_KEY_KP_1            ::Se::Key::KP1
#define SE_KEY_KP_2            ::Se::Key::KP2
#define SE_KEY_KP_3            ::Se::Key::KP3
#define SE_KEY_KP_4            ::Se::Key::KP4
#define SE_KEY_KP_5            ::Se::Key::KP5
#define SE_KEY_KP_6            ::Se::Key::KP6
#define SE_KEY_KP_7            ::Se::Key::KP7
#define SE_KEY_KP_8            ::Se::Key::KP8
#define SE_KEY_KP_9            ::Se::Key::KP9
#define SE_KEY_KP_DECIMAL      ::Se::Key::KPDecimal
#define SE_KEY_KP_DIVIDE       ::Se::Key::KPDivide
#define SE_KEY_KP_MULTIPLY     ::Se::Key::KPMultiply
#define SE_KEY_KP_SUBTRACT     ::Se::Key::KPSubtract
#define SE_KEY_KP_ADD          ::Se::Key::KPAdd
#define SE_KEY_KP_ENTER        ::Se::Key::KPEnter
#define SE_KEY_KP_EQUAL        ::Se::Key::KPEqual

#define SE_KEY_LEFT_SHIFT      ::Se::Key::LeftShift
#define SE_KEY_LEFT_CONTROL    ::Se::Key::LeftControl
#define SE_KEY_LEFT_ALT        ::Se::Key::LeftAlt
#define SE_KEY_LEFT_SUPER      ::Se::Key::LeftSuper
#define SE_KEY_RIGHT_SHIFT     ::Se::Key::RightShift
#define SE_KEY_RIGHT_CONTROL   ::Se::Key::RightControl
#define SE_KEY_RIGHT_ALT       ::Se::Key::RightAlt
#define SE_KEY_RIGHT_SUPER     ::Se::Key::RightSuper
#define SE_KEY_MENU            ::Se::Key::Menu
