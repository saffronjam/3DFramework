#include "SaffronPCH.h"

#include "Saffron/Common/App.h"
#include "Saffron/Input/Mouse.h"

namespace Se
{
SignalAggregate<MouseButtonCode> Mouse::Signals::OnPressed;
SignalAggregate<MouseButtonCode> Mouse::Signals::OnReleased;
SignalAggregate<float> Mouse::Signals::OnScrolled;
SignalAggregate<Vector2> Mouse::Signals::OnMoved;
SignalAggregate<void> Mouse::Signals::OnEntered;
SignalAggregate<void> Mouse::Signals::OnLeft;

Mouse* Mouse::_instance = nullptr;

Mouse::Mouse()
{
	SE_CORE_ASSERT(_instance == nullptr, "Mouse was already instansiated");
	_instance = this;
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

void Mouse::OnEvent(const Event& event)
{
	const auto dispatcher = EventDispatcher(event);

	dispatcher.Try<MouseButtonPressedEvent>(SE_BIND_EVENT_FN(Mouse::OnMouseButtonPressed));
	dispatcher.Try<MouseButtonReleasedEvent>(SE_BIND_EVENT_FN(Mouse::OnMouseButtonReleased));
	dispatcher.Try<MouseWheelScrolledEvent>(SE_BIND_EVENT_FN(Mouse::OnMouseWheelScrolled));
	dispatcher.Try<MouseMovedEvent>(SE_BIND_EVENT_FN(Mouse::OnMouseMoved));
	dispatcher.Try<CursorEnteredEvent>(SE_BIND_EVENT_FN(Mouse::OnCursorEntered));
	dispatcher.Try<CursorLeftEvent>(SE_BIND_EVENT_FN(Mouse::OnCursorLeft));
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

Vector2 Mouse::GetPosition()
{
	return Instance()._position;
}

Vector2 Mouse::GetSwipe()
{
	Vector2 swipe = Instance()._position - Instance()._lastPosition;
	if (swipe.LengthSquared() > 1000.0f)
	{
		swipe = Vector2::Zero;
	}
	return swipe;
}

bool Mouse::IsCursorEnabled()
{
	return Instance()._cursorEnabled;
}

void Mouse::EnableCursor()
{
	App::Get().GetWindow()->EnableCursor();
}

void Mouse::DisableCursor()
{
	App::Get().GetWindow()->DisableCursor();
}

bool Mouse::IsCursorVisible()
{
	return Instance()._cursorVisible;
}

void Mouse::ShowCursor()
{
	App::Get().GetWindow()->ShowCursor();
}

void Mouse::HideCursor()
{
	App::Get().GetWindow()->HideCursor();
}

bool Mouse::IsCursorConfined()
{
	return Instance()._cursorConfined;
}

void Mouse::ConfineCursor()
{
	App::Get().GetWindow()->ConfineCursor();
}

void Mouse::FreeCursor()
{
	App::Get().GetWindow()->FreeCursor();
}

bool Mouse::InWindow()
{
	return Instance()._inWindow;
}

bool Mouse::IsRawInputEnabled()
{
	return Instance()._rawInput;
}

void Mouse::EnableRawInput()
{
}

void Mouse::DisableRawInput()
{
}

bool Mouse::OnMouseButtonPressed(const MouseButtonPressedEvent& event)
{
	GetSignals().Emit(Signals::OnPressed, event.GetButton());
	_mouseState[event.GetButton()] = true;
	return false;
}

bool Mouse::OnMouseButtonReleased(const MouseButtonReleasedEvent& event)
{
	GetSignals().Emit(Signals::OnPressed, event.GetButton());
	_mouseState[event.GetButton()] = false;
	return false;
}

bool Mouse::OnMouseWheelScrolled(const MouseWheelScrolledEvent& event)
{
	GetSignals().Emit(Signals::OnScrolled, event.GetOffset());
	return false;
}

bool Mouse::OnMouseMoved(const MouseMovedEvent& event)
{
	GetSignals().Emit(Signals::OnMoved, event.GetPosition());
	_lastPosition = _position;
	_position = event.GetPosition();
	return false;
}

bool Mouse::OnCursorEntered(const CursorEnteredEvent& event)
{
	GetSignals().Emit<>(Signals::OnEntered);
	_inWindow = true;
	return false;
}

bool Mouse::OnCursorLeft(const CursorLeftEvent& event)
{
	GetSignals().Emit(Signals::OnLeft);
	_inWindow = false;
	return false;
}
}
