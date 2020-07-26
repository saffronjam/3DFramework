#pragma once

#include "Saffron/SaffronPCH.h"

namespace Saffron
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


#define SE_KEY_SPACE           ::Saffron::Key::Space
#define SE_KEY_APOSTROPHE      ::Saffron::Key::Apostrophe    /* ' */
#define SE_KEY_COMMA           ::Saffron::Key::Comma         /* , */
#define SE_KEY_MINUS           ::Saffron::Key::Minus         /* - */
#define SE_KEY_PERIOD          ::Saffron::Key::Period        /* . */
#define SE_KEY_SLASH           ::Saffron::Key::Slash         /* / */
#define SE_KEY_0               ::Saffron::Key::D0
#define SE_KEY_1               ::Saffron::Key::D1
#define SE_KEY_2               ::Saffron::Key::D2
#define SE_KEY_3               ::Saffron::Key::D3
#define SE_KEY_4               ::Saffron::Key::D4
#define SE_KEY_5               ::Saffron::Key::D5
#define SE_KEY_6               ::Saffron::Key::D6
#define SE_KEY_7               ::Saffron::Key::D7
#define SE_KEY_8               ::Saffron::Key::D8
#define SE_KEY_9               ::Saffron::Key::D9
#define SE_KEY_SEMICOLON       ::Saffron::Key::Semicolon     /* ; */
#define SE_KEY_EQUAL           ::Saffron::Key::Equal         /* = */
#define SE_KEY_A               ::Saffron::Key::A
#define SE_KEY_B               ::Saffron::Key::B
#define SE_KEY_C               ::Saffron::Key::C
#define SE_KEY_D               ::Saffron::Key::D
#define SE_KEY_E               ::Saffron::Key::E
#define SE_KEY_F               ::Saffron::Key::F
#define SE_KEY_G               ::Saffron::Key::G
#define SE_KEY_H               ::Saffron::Key::H
#define SE_KEY_I               ::Saffron::Key::I
#define SE_KEY_J               ::Saffron::Key::J
#define SE_KEY_K               ::Saffron::Key::K
#define SE_KEY_L               ::Saffron::Key::L
#define SE_KEY_M               ::Saffron::Key::M
#define SE_KEY_N               ::Saffron::Key::N
#define SE_KEY_O               ::Saffron::Key::O
#define SE_KEY_P               ::Saffron::Key::P
#define SE_KEY_Q               ::Saffron::Key::Q
#define SE_KEY_R               ::Saffron::Key::R
#define SE_KEY_S               ::Saffron::Key::S
#define SE_KEY_T               ::Saffron::Key::T
#define SE_KEY_U               ::Saffron::Key::U
#define SE_KEY_V               ::Saffron::Key::V
#define SE_KEY_W               ::Saffron::Key::W
#define SE_KEY_X               ::Saffron::Key::X
#define SE_KEY_Y               ::Saffron::Key::Y
#define SE_KEY_Z               ::Saffron::Key::Z
#define SE_KEY_LEFT_BRACKET    ::Saffron::Key::LeftBracket   /* [ */
#define SE_KEY_BACKSLASH       ::Saffron::Key::Backslash     /* \ */
#define SE_KEY_RIGHT_BRACKET   ::Saffron::Key::RightBracket  /* ] */
#define SE_KEY_GRAVE_ACCENT    ::Saffron::Key::GraveAccent   /* ` */
#define SE_KEY_WORLD_1         ::Saffron::Key::World1        /* non-US #1 */
#define SE_KEY_WORLD_2         ::Saffron::Key::World2        /* non-US #2 */

/* Function keys */
#define SE_KEY_ESCAPE          ::Saffron::Key::Escape
#define SE_KEY_ENTER           ::Saffron::Key::Enter
#define SE_KEY_TAB             ::Saffron::Key::Tab
#define SE_KEY_BACKSPACE       ::Saffron::Key::Backspace
#define SE_KEY_INSERT          ::Saffron::Key::Insert
#define SE_KEY_DELETE          ::Saffron::Key::Delete
#define SE_KEY_RIGHT           ::Saffron::Key::Right
#define SE_KEY_LEFT            ::Saffron::Key::Left
#define SE_KEY_DOWN            ::Saffron::Key::Down
#define SE_KEY_UP              ::Saffron::Key::Up
#define SE_KEY_PAGE_UP         ::Saffron::Key::PageUp
#define SE_KEY_PAGE_DOWN       ::Saffron::Key::PageDown
#define SE_KEY_HOME            ::Saffron::Key::Home
#define SE_KEY_END             ::Saffron::Key::End
#define SE_KEY_CAPS_LOCK       ::Saffron::Key::CapsLock
#define SE_KEY_SCROLL_LOCK     ::Saffron::Key::ScrollLock
#define SE_KEY_NUM_LOCK        ::Saffron::Key::NumLock
#define SE_KEY_PRINT_SCREEN    ::Saffron::Key::PrintScreen
#define SE_KEY_PAUSE           ::Saffron::Key::Pause
#define SE_KEY_F1              ::Saffron::Key::F1
#define SE_KEY_F2              ::Saffron::Key::F2
#define SE_KEY_F3              ::Saffron::Key::F3
#define SE_KEY_F4              ::Saffron::Key::F4
#define SE_KEY_F5              ::Saffron::Key::F5
#define SE_KEY_F6              ::Saffron::Key::F6
#define SE_KEY_F7              ::Saffron::Key::F7
#define SE_KEY_F8              ::Saffron::Key::F8
#define SE_KEY_F9              ::Saffron::Key::F9
#define SE_KEY_F10             ::Saffron::Key::F10
#define SE_KEY_F11             ::Saffron::Key::F11
#define SE_KEY_F12             ::Saffron::Key::F12
#define SE_KEY_F13             ::Saffron::Key::F13
#define SE_KEY_F14             ::Saffron::Key::F14
#define SE_KEY_F15             ::Saffron::Key::F15
#define SE_KEY_F16             ::Saffron::Key::F16
#define SE_KEY_F17             ::Saffron::Key::F17
#define SE_KEY_F18             ::Saffron::Key::F18
#define SE_KEY_F19             ::Saffron::Key::F19
#define SE_KEY_F20             ::Saffron::Key::F20
#define SE_KEY_F21             ::Saffron::Key::F21
#define SE_KEY_F22             ::Saffron::Key::F22
#define SE_KEY_F23             ::Saffron::Key::F23
#define SE_KEY_F24             ::Saffron::Key::F24
#define SE_KEY_F25             ::Saffron::Key::F25

/* Keypad */
#define SE_KEY_KP_0            ::Saffron::Key::KP0
#define SE_KEY_KP_1            ::Saffron::Key::KP1
#define SE_KEY_KP_2            ::Saffron::Key::KP2
#define SE_KEY_KP_3            ::Saffron::Key::KP3
#define SE_KEY_KP_4            ::Saffron::Key::KP4
#define SE_KEY_KP_5            ::Saffron::Key::KP5
#define SE_KEY_KP_6            ::Saffron::Key::KP6
#define SE_KEY_KP_7            ::Saffron::Key::KP7
#define SE_KEY_KP_8            ::Saffron::Key::KP8
#define SE_KEY_KP_9            ::Saffron::Key::KP9
#define SE_KEY_KP_DECIMAL      ::Saffron::Key::KPDecimal
#define SE_KEY_KP_DIVIDE       ::Saffron::Key::KPDivide
#define SE_KEY_KP_MULTIPLY     ::Saffron::Key::KPMultiply
#define SE_KEY_KP_SUBTRACT     ::Saffron::Key::KPSubtract
#define SE_KEY_KP_ADD          ::Saffron::Key::KPAdd
#define SE_KEY_KP_ENTER        ::Saffron::Key::KPEnter
#define SE_KEY_KP_EQUAL        ::Saffron::Key::KPEqual

#define SE_KEY_LEFT_SHIFT      ::Saffron::Key::LeftShift
#define SE_KEY_LEFT_CONTROL    ::Saffron::Key::LeftControl
#define SE_KEY_LEFT_ALT        ::Saffron::Key::LeftAlt
#define SE_KEY_LEFT_SUPER      ::Saffron::Key::LeftSuper
#define SE_KEY_RIGHT_SHIFT     ::Saffron::Key::RightShift
#define SE_KEY_RIGHT_CONTROL   ::Saffron::Key::RightControl
#define SE_KEY_RIGHT_ALT       ::Saffron::Key::RightAlt
#define SE_KEY_RIGHT_SUPER     ::Saffron::Key::RightSuper
#define SE_KEY_MENU            ::Saffron::Key::Menu