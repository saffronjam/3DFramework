#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/MouseEvent.h"
#include "Saffron/Input/InputCodes.h"

namespace Se
{
class Mouse : public Managed
{
public:
	Mouse();

	void OnUpdate();

	static bool IsPressed(MouseButtonCode mouseButtonCode);
	static bool IsReleased(MouseButtonCode mouseButtonCode);
	static bool IsDown(MouseButtonCode mouseButtonCode);

	static bool GetScroll(MouseWheelCode mouseWheelCode);
	static Vector2 GetPosition();
	static Vector2 GetPositionNDC();
	static Vector2 GetSwipe();

	static bool IsCursorEnabled();
	static void EnableCursor();
	static void DisableCursor();

	static bool IsCursorVisible();
	static void ShowCursor();
	static void HideCursor();

	static bool IsCursorConfined();
	static void ConfineCursor();
	static void FreeCursor();

	static bool InWindow();

	static bool IsRawInputEnabled();
	static void EnableRawInput();
	static void DisableRawInput();

private:
	bool OnMouseButtonPressed(const MouseButtonPressedEvent& event);
	bool OnMouseButtonReleased(const MouseButtonReleasedEvent& event);
	bool OnMouseWheelScrolled(const MouseWheelScrolledEvent& event);
	bool OnMouseMoved(const MouseMovedEvent& event);
	bool OnCursorEntered(const CursorEnteredEvent& event);
	bool OnCursorLeft(const CursorLeftEvent& event);

	static Mouse& Instance()
	{
		Debug::Assert(_instance != nullptr && "Mouse was not instansiated");
		return *_instance;
	}

	static bool Instansiated()
	{
		return _instance != nullptr;
	}

public:
	static EventSubscriberList<const MouseButtonPressedEvent&> ButtonPressed;
	static EventSubscriberList<const MouseButtonReleasedEvent&> ButtonReleased;
	static EventSubscriberList<const MouseWheelScrolledEvent&> WheelScrolled;
	static EventSubscriberList<const MouseMovedEvent&> Moved;
	static EventSubscriberList<const MouseMovedRawEvent&> MovedRaw;
	static EventSubscriberList<const CursorEnteredEvent&> CursorEntered;
	static EventSubscriberList<const CursorLeftEvent&> CursorLeft;

private:
	HashMap<MouseButtonCode, bool> _mouseState;
	HashMap<MouseButtonCode, bool> _prevMouseState;
	Array<int, static_cast<size_t>(MouseWheelCode::Count)> _mouseWheelScrolls = {};

	bool _cursorEnabled = true;
	bool _cursorConfined = false;
	bool _cursorVisible = true;
	bool _inWindow = true;
	bool _rawInput = false;

	Vector2 _position;
	Vector2 _lastPosition;

	static Mouse* _instance;
};
}
