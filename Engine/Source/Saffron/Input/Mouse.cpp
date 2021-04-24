#include "SaffronPCH.h"

#include "Saffron/Core/App.h"
#include "Saffron/Input/Mouse.h"

namespace Se
{
Mouse* Mouse::_instance = nullptr;

EventSubscriberList<const MouseButtonPressedEvent&> Mouse::ButtonPressed;
EventSubscriberList<const MouseButtonReleasedEvent&> Mouse::ButtonReleased;
EventSubscriberList<const MouseWheelScrolledEvent&> Mouse::WheelScrolled;
EventSubscriberList<const MouseMovedEvent&> Mouse::Moved;
EventSubscriberList<const MouseMovedRawEvent&> Mouse::MovedRaw;
EventSubscriberList<const CursorEnteredEvent&> Mouse::CursorEntered;
EventSubscriberList<const CursorLeftEvent&> Mouse::CursorLeft;

Mouse::Mouse()
{
	Debug::Assert(_instance == nullptr, "Mouse was already instansiated");;
	_instance = this;

	const auto& window = App::Instance().GetWindow();
	window.MouseButtonPressed += SE_BIND_EVENT_FN(Mouse::OnMouseButtonPressed);
	window.MouseButtonReleased += SE_BIND_EVENT_FN(Mouse::OnMouseButtonReleased);
	window.MouseWheelScrolled += SE_BIND_EVENT_FN(Mouse::OnMouseWheelScrolled);
	window.MouseMoved += SE_BIND_EVENT_FN(Mouse::OnMouseMoved);
	window.CursorEntered += SE_BIND_EVENT_FN(Mouse::OnCursorEntered);
	window.CursorLeft += SE_BIND_EVENT_FN(Mouse::OnCursorLeft);

	FreeCursor();
	ShowCursor();
}

void Mouse::OnUpdate()
{
	_lastPosition = _position;

	for (auto& [button, state] : _mouseState)
	{
		_prevMouseState[button] = state;
	}

	for (auto& scroll : _mouseWheelScrolls)
	{
		scroll = 0;
	}
}

bool Mouse::IsPressed(MouseButtonCode mouseButtonCode)
{
	return Instance()._mouseState[mouseButtonCode] && !Instance()._prevMouseState[mouseButtonCode];
}

bool Mouse::IsReleased(MouseButtonCode mouseButtonCode)
{
	return !Instance()._mouseState[mouseButtonCode] && Instance()._prevMouseState[mouseButtonCode];
}

bool Mouse::IsDown(MouseButtonCode mouseButtonCode)
{
	return Instance()._mouseState[mouseButtonCode];
}

bool Mouse::GetScroll(MouseWheelCode mouseWheelCode)
{
	return Instance()._mouseWheelScrolls[static_cast<int>(mouseWheelCode)];
}

Vector2f Mouse::GetPosition()
{
	return Instance()._position;
}

Vector2f Mouse::GetPositionNDC()
{
	const ImVec2 mousePos = ImGui::GetMousePos();
	return {mousePos.x, mousePos.y};
}

Vector2f Mouse::GetSwipe()
{
	Vector2f swipe = Instance()._position - Instance()._lastPosition;
	if (length2(swipe) > 10000.0f)
	{
		swipe = Vector2f(0);
	}
	return swipe;
}

bool Mouse::IsCursorEnabled()
{
	return Instansiated() && Instance()._cursorEnabled;
}

void Mouse::EnableCursor()
{
}

void Mouse::DisableCursor()
{
}

bool Mouse::IsCursorVisible()
{
	return Instansiated() && Instance()._cursorVisible;
}

void Mouse::ShowCursor()
{
}

void Mouse::HideCursor()
{
}

bool Mouse::IsCursorConfined()
{
	return Instansiated() && Instance()._cursorConfined;
}

void Mouse::ConfineCursor()
{
}

void Mouse::FreeCursor()
{
}

bool Mouse::InWindow()
{
	return Instansiated() && Instance()._inWindow;
}

bool Mouse::IsRawInputEnabled()
{
	return Instansiated() && Instance()._rawInput;
}

void Mouse::EnableRawInput()
{
}

void Mouse::DisableRawInput()
{
}

bool Mouse::OnMouseButtonPressed(const MouseButtonPressedEvent& event)
{
	ButtonPressed.Invoke(event);
	_mouseState[event.GetButton()] = true;
	return false;
}

bool Mouse::OnMouseButtonReleased(const MouseButtonReleasedEvent& event)
{
	ButtonReleased.Invoke(event);
	_mouseState[event.GetButton()] = false;
	return false;
}

bool Mouse::OnMouseWheelScrolled(const MouseWheelScrolledEvent& event)
{
	WheelScrolled.Invoke(event);
	return false;
}

bool Mouse::OnMouseMoved(const MouseMovedEvent& event)
{
	Moved.Invoke(event);
	_lastPosition = _position;
	_position = event.GetPosition();
	return false;
}

bool Mouse::OnCursorEntered(const CursorEnteredEvent& event)
{
	CursorEntered.Invoke(event);
	_inWindow = true;
	return false;
}

bool Mouse::OnCursorLeft(const CursorLeftEvent& event)
{
	CursorLeft.Invoke(event);
	_inWindow = false;
	return false;
}
}
