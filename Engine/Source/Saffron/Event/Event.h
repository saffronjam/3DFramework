#pragma once

#include "Saffron/Core/TypeDefs.h"
#include "Saffron/Input/Codes.h"

namespace Se
{
////////////////////////////////////////////////////////////
/// \brief Enumeration of the different types of events
////////////////////////////////////////////////////////////
enum class EventType
{
	Closed,
	//!< The window requested to be closed (no data)
	Resized,
	//!< The window was resized (data in event.size)
	LostFocus,
	//!< The window lost the focus (no data)
	GainedFocus,
	//!< The window gained the focus (no data)
	TextEntered,
	//!< A character was entered (data in event.text)
	KeyPressed,
	//!< A key was pressed (data in event.key)
	KeyReleased,
	//!< A key was released (data in event.key)
	MouseWheelScrolled,
	//!< The mouse Wheel was scrolled (data in event.mouseWheelScroll)
	MouseButtonPressed,
	//!< A mouse Button was pressed (data in event.mouseButton)
	MouseButtonReleased,
	//!< A mouse Button was released (data in event.mouseButton)
	MouseMoved,
	//!< The mouse cursor moved (data in event.mouseMove)
	MouseEntered,
	//!< The mouse cursor entered the area of the window (no data)
	MouseLeft,
	//!< The mouse cursor left the area of the window (no data)

	Count //!< Keep last -- the total number of event types
};

class Event
{
public:
	////////////////////////////////////////////////////////////
	/// \brief Size events parameters (Resized)
	////////////////////////////////////////////////////////////
	struct SizeEvent
	{
		unsigned int Width; //!< New Width, in pixels
		unsigned int Height; //!< New Height, in pixels
	};

	////////////////////////////////////////////////////////////
	/// \brief Keyboard event parameters (KeyPressed, KeyReleased)
	////////////////////////////////////////////////////////////
	struct KeyEvent
	{
		KeyCode Code; //!< Code of the key that has been pressed
		bool Alt; //!< Is the Alt key pressed?
		bool Control; //!< Is the Control key pressed?
		bool Shift; //!< Is the Shift key pressed?
		bool System; //!< Is the System key pressed?
	};

	////////////////////////////////////////////////////////////
	/// \brief Text event parameters (TextEntered)
	////////////////////////////////////////////////////////////
	struct TextEvent
	{
		uint Unicode; //!< UTF-32 Unicode value of the character
	};

	////////////////////////////////////////////////////////////
	/// \brief Mouse move event parameters (MouseMoved)
	////////////////////////////////////////////////////////////
	struct MouseMoveEvent
	{
		int x; //!< X position of the mouse pointer, relative to the left of the owner window
		int y; //!< Y position of the mouse pointer, relative to the top of the owner window
	};

	////////////////////////////////////////////////////////////
	/// \brief Mouse buttons events parameters
	///        (MouseButtonPressed, MouseButtonReleased)
	////////////////////////////////////////////////////////////
	struct MouseButtonEvent
	{
		MouseButtonCode Button; //!< Code of the Button that has been pressed
		int x; //!< X position of the mouse pointer, relative to the left of the owner window
		int y; //!< Y position of the mouse pointer, relative to the top of the owner window
	};

	////////////////////////////////////////////////////////////
	/// \brief Mouse Wheel events parameters (MouseWheelScrolled)
	////////////////////////////////////////////////////////////
	struct MouseWheelScrollEvent
	{
		MouseWheelCode Wheel; //!< Which Wheel (for mice with multiple ones)
		float Delta;
		//!< Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
		int x; //!< X position of the mouse pointer, relative to the left of the owner window
		int y; //!< Y position of the mouse pointer, relative to the top of the owner window
	};


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////
	EventType Type; //!< Type of the event

	union
	{
		SizeEvent size; //!< Size event parameters (Event::Resized)
		KeyEvent key; //!< Key event parameters (Event::KeyPressed, Event::KeyReleased)
		TextEvent text; //!< Text event parameters (Event::TextEntered)
		MouseMoveEvent mouseMove; //!< Mouse move event parameters (Event::MouseMoved)
		MouseButtonEvent mouseButton;
		//!< Mouse Button event parameters (Event::MouseButtonPressed, Event::MouseButtonReleased)
		MouseWheelScrollEvent mouseWheelScroll; //!< Mouse Wheel event parameters (Event::MouseWheelScrolled)
	};
};
}
