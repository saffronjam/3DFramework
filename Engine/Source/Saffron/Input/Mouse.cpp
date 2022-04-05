#include "SaffronPCH.h"

#include "Saffron/Input/Mouse.h"

#include <imgui.h>

#include "Saffron/Common/App.h"

namespace Se
{
Mouse::Mouse() :
	Singleton(this)
{
	auto& win = App::Instance().Window();
	win.MouseWheelScrolled += SE_EV_ACTION(Mouse::OnScroll);
	win.MouseButtonPressed += SE_EV_ACTION(Mouse::OnButtonPress);
	win.MouseButtonReleased += SE_EV_ACTION(Mouse::OnButtonRelease);
	win.MouseMoved += SE_EV_ACTION(Mouse::OnMove);
	win.MouseEntered += SE_EV_ACTION(Mouse::OnCursorEnter);
	win.MouseLeft += SE_EV_ACTION(Mouse::OnCursorLeave);
}

void Mouse::OnUpdate()
{
	auto& inst = Instance();

	inst._prevButtonStates = inst._buttonStates;

	inst._scrollBuffer = 0.0f;
	inst._mouseSwipe = Vector2{0.0f, 0.0f};
}


auto Mouse::IsButtonDown(const MouseButtonCode& button) -> bool
{
	auto& inst = Instance();

	return inst._buttonStates[button];
}

auto Mouse::IsButtonPressed(const MouseButtonCode& button) -> bool
{
	auto& inst = Instance();

	return inst._buttonStates[button] && !inst._prevButtonStates[button];
}

auto Mouse::IsButtonReleased(const MouseButtonCode& button) -> bool
{
	auto& inst = Instance();

	return !inst._buttonStates[button] && inst._prevButtonStates[button];
}

auto Mouse::IsAnyButtonDown() -> bool
{
	const auto& inst = Instance();
	for (const auto state : inst._buttonStates | std::views::values)
	{
		if (state)
		{
			return true;
		}
	}
	return false;
}

auto Mouse::IsCursorEnabled() -> bool
{
	return Instance()._cursorEnabled;
}

auto Mouse::IsCursorInWindow() -> bool
{
	return Instance()._inWindow;
}

auto Mouse::CursorPosition(bool normalized) -> const Vector2&
{
	auto& inst = Instance();

	if (!normalized)
	{
		return inst._mousePosition;
	}
	return inst._mousePositionNDC;
}

auto Mouse::Swipe() -> const Vector2&
{
	auto& inst = Instance();

	return inst._mouseSwipe;
}

auto Mouse::Scroll() -> float
{
	auto& inst = Instance();

	return inst._scrollBuffer;
}

void Mouse::OnButtonPress(const MouseButtonEvent& event)
{
	auto& inst = Instance();

	inst._buttonStates[event.Button] = true;
}

void Mouse::OnButtonRelease(const MouseButtonEvent& event)
{
	auto& inst = Instance();

	inst._buttonStates[event.Button] = false;
}

void Mouse::OnMove(const MouseMoveEvent& event)
{
	auto& inst = Instance();

	if (!inst._inWindow && IsAnyButtonDown() || inst._inWindow)
	{
		const auto newPos = Vector2(event.x, event.y);
		inst._mouseSwipe = inst._mousePosition - newPos;
		inst._mousePosition = newPos;
		const auto& win = App::Instance().Window();
		inst._mousePositionNDC = Vector2(newPos.x / win.Width() * 2.0f - 1.0f, newPos.y / win.Height() * 2.0f - 1.0f);
	}
}

void Mouse::OnCursorEnter()
{
	auto& inst = Instance();

	inst._inWindow = true;
}

void Mouse::OnCursorLeave()
{
	auto& inst = Instance();

	inst._inWindow = false;
}

void Mouse::OnScroll(const MouseWheelScrollEvent& event)
{
	auto& inst = Instance();

	inst._scrollBuffer += event.Delta;
}
}
