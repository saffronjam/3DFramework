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
	D0 = 48,
	D1 = 49,
	D2 = 50,
	D3 = 51,
	D4 = 52,
	D5 = 53,
	D6 = 54,
	D7 = 55,
	D8 = 56,
	D9 = 57,
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


#define SE_KEY_SPACE           ::Se::KeyCode::Space
#define SE_KEY_APOSTROPHE      ::Se::KeyCode::Apostrophe    /* ' */
#define SE_KEY_COMMA           ::Se::KeyCode::Comma         /* , */
#define SE_KEY_MINUS           ::Se::KeyCode::Minus         /* - */
#define SE_KEY_PERIOD          ::Se::KeyCode::Period        /* . */
#define SE_KEY_SLASH           ::Se::KeyCode::Slash         /* / */
#define SE_KEY_0               ::Se::KeyCode::D0
#define SE_KEY_1               ::Se::KeyCode::D1
#define SE_KEY_2               ::Se::KeyCode::D2
#define SE_KEY_3               ::Se::KeyCode::D3
#define SE_KEY_4               ::Se::KeyCode::D4
#define SE_KEY_5               ::Se::KeyCode::D5
#define SE_KEY_6               ::Se::KeyCode::D6
#define SE_KEY_7               ::Se::KeyCode::D7
#define SE_KEY_8               ::Se::KeyCode::D8
#define SE_KEY_9               ::Se::KeyCode::D9
#define SE_KEY_SEMICOLON       ::Se::KeyCode::Semicolon     /* ; */
#define SE_KEY_EQUAL           ::Se::KeyCode::Equal         /* = */
#define SE_KEY_A               ::Se::KeyCode::A
#define SE_KEY_B               ::Se::KeyCode::B
#define SE_KEY_C               ::Se::KeyCode::C
#define SE_KEY_D               ::Se::KeyCode::D
#define SE_KEY_E               ::Se::KeyCode::E
#define SE_KEY_F               ::Se::KeyCode::F
#define SE_KEY_G               ::Se::KeyCode::G
#define SE_KEY_H               ::Se::KeyCode::H
#define SE_KEY_I               ::Se::KeyCode::I
#define SE_KEY_J               ::Se::KeyCode::J
#define SE_KEY_K               ::Se::KeyCode::K
#define SE_KEY_L               ::Se::KeyCode::L
#define SE_KEY_M               ::Se::KeyCode::M
#define SE_KEY_N               ::Se::KeyCode::N
#define SE_KEY_O               ::Se::KeyCode::O
#define SE_KEY_P               ::Se::KeyCode::P
#define SE_KEY_Q               ::Se::KeyCode::Q
#define SE_KEY_R               ::Se::KeyCode::R
#define SE_KEY_S               ::Se::KeyCode::S
#define SE_KEY_T               ::Se::KeyCode::T
#define SE_KEY_U               ::Se::KeyCode::U
#define SE_KEY_V               ::Se::KeyCode::V
#define SE_KEY_W               ::Se::KeyCode::W
#define SE_KEY_X               ::Se::KeyCode::X
#define SE_KEY_Y               ::Se::KeyCode::Y
#define SE_KEY_Z               ::Se::KeyCode::Z
#define SE_KEY_LEFT_BRACKET    ::Se::KeyCode::LeftBracket   /* [ */
#define SE_KEY_BACKSLASH       ::Se::KeyCode::Backslash     /* \ */
#define SE_KEY_RIGHT_BRACKET   ::Se::KeyCode::RightBracket  /* ] */
#define SE_KEY_GRAVE_ACCENT    ::Se::KeyCode::GraveAccent   /* ` */
#define SE_KEY_WORLD_1         ::Se::KeyCode::World1        /* non-US #1 */
#define SE_KEY_WORLD_2         ::Se::KeyCode::World2        /* non-US #2 */

/* Function keys */
#define SE_KEY_ESCAPE          ::Se::KeyCode::Escape
#define SE_KEY_ENTER           ::Se::KeyCode::Enter
#define SE_KEY_TAB             ::Se::KeyCode::Tab
#define SE_KEY_BACKSPACE       ::Se::KeyCode::Backspace
#define SE_KEY_INSERT          ::Se::KeyCode::Insert
#define SE_KEY_DELETE          ::Se::KeyCode::Delete
#define SE_KEY_RIGHT           ::Se::KeyCode::Right
#define SE_KEY_LEFT            ::Se::KeyCode::Left
#define SE_KEY_DOWN            ::Se::KeyCode::Down
#define SE_KEY_UP              ::Se::KeyCode::Up
#define SE_KEY_PAGE_UP         ::Se::KeyCode::PageUp
#define SE_KEY_PAGE_DOWN       ::Se::KeyCode::PageDown
#define SE_KEY_HOME            ::Se::KeyCode::Home
#define SE_KEY_END             ::Se::KeyCode::End
#define SE_KEY_CAPS_LOCK       ::Se::KeyCode::CapsLock
#define SE_KEY_SCROLL_LOCK     ::Se::KeyCode::ScrollLock
#define SE_KEY_NUM_LOCK        ::Se::KeyCode::NumLock
#define SE_KEY_PRINT_SCREEN    ::Se::KeyCode::PrintScreen
#define SE_KEY_PAUSE           ::Se::KeyCode::Pause
#define SE_KEY_F1              ::Se::KeyCode::F1
#define SE_KEY_F2              ::Se::KeyCode::F2
#define SE_KEY_F3              ::Se::KeyCode::F3
#define SE_KEY_F4              ::Se::KeyCode::F4
#define SE_KEY_F5              ::Se::KeyCode::F5
#define SE_KEY_F6              ::Se::KeyCode::F6
#define SE_KEY_F7              ::Se::KeyCode::F7
#define SE_KEY_F8              ::Se::KeyCode::F8
#define SE_KEY_F9              ::Se::KeyCode::F9
#define SE_KEY_F10             ::Se::KeyCode::F10
#define SE_KEY_F11             ::Se::KeyCode::F11
#define SE_KEY_F12             ::Se::KeyCode::F12
#define SE_KEY_F13             ::Se::KeyCode::F13
#define SE_KEY_F14             ::Se::KeyCode::F14
#define SE_KEY_F15             ::Se::KeyCode::F15
#define SE_KEY_F16             ::Se::KeyCode::F16
#define SE_KEY_F17             ::Se::KeyCode::F17
#define SE_KEY_F18             ::Se::KeyCode::F18
#define SE_KEY_F19             ::Se::KeyCode::F19
#define SE_KEY_F20             ::Se::KeyCode::F20
#define SE_KEY_F21             ::Se::KeyCode::F21
#define SE_KEY_F22             ::Se::KeyCode::F22
#define SE_KEY_F23             ::Se::KeyCode::F23
#define SE_KEY_F24             ::Se::KeyCode::F24
#define SE_KEY_F25             ::Se::KeyCode::F25

/* Keypad */
#define SE_KEY_KP_0            ::Se::KeyCode::KP0
#define SE_KEY_KP_1            ::Se::KeyCode::KP1
#define SE_KEY_KP_2            ::Se::KeyCode::KP2
#define SE_KEY_KP_3            ::Se::KeyCode::KP3
#define SE_KEY_KP_4            ::Se::KeyCode::KP4
#define SE_KEY_KP_5            ::Se::KeyCode::KP5
#define SE_KEY_KP_6            ::Se::KeyCode::KP6
#define SE_KEY_KP_7            ::Se::KeyCode::KP7
#define SE_KEY_KP_8            ::Se::KeyCode::KP8
#define SE_KEY_KP_9            ::Se::KeyCode::KP9
#define SE_KEY_KP_DECIMAL      ::Se::KeyCode::KPDecimal
#define SE_KEY_KP_DIVIDE       ::Se::KeyCode::KPDivide
#define SE_KEY_KP_MULTIPLY     ::Se::KeyCode::KPMultiply
#define SE_KEY_KP_SUBTRACT     ::Se::KeyCode::KPSubtract
#define SE_KEY_KP_ADD          ::Se::KeyCode::KPAdd
#define SE_KEY_KP_ENTER        ::Se::KeyCode::KPEnter
#define SE_KEY_KP_EQUAL        ::Se::KeyCode::KPEqual

#define SE_KEY_LEFT_SHIFT      ::Se::KeyCode::LeftShift
#define SE_KEY_LEFT_CONTROL    ::Se::KeyCode::LeftControl
#define SE_KEY_LEFT_ALT        ::Se::KeyCode::LeftAlt
#define SE_KEY_LEFT_SUPER      ::Se::KeyCode::LeftSuper
#define SE_KEY_RIGHT_SHIFT     ::Se::KeyCode::RightShift
#define SE_KEY_RIGHT_CONTROL   ::Se::KeyCode::RightControl
#define SE_KEY_RIGHT_ALT       ::Se::KeyCode::RightAlt
#define SE_KEY_RIGHT_SUPER     ::Se::KeyCode::RightSuper
#define SE_KEY_MENU            ::Se::KeyCode::Menu
