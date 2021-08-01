#include "SaffronPCH.h"

#include "Saffron/Input/Mouse.h"

#include <imgui.h>

#include "Saffron/Common/App.h"

namespace Se
{
Mouse::Mouse() :
	SingleTon(this)
{
	for (int i = 0; i < static_cast<int>(MouseButtonCode::Count); i++)
	{
		_buttonStates[i] = false;
		_prevButtonStates[i] = false;
	}


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

	inst._horizontalScrollBuffer = 0.0f;
	inst._verticalScrollBuffer = 0.0f;
	inst._mouseSwipe = Vector2{0.0f, 0.0f};
}


auto Mouse::IsButtonDown(const MouseButtonCode& button) -> bool
{
	auto& inst = Instance();

	return inst._buttonStates[static_cast<int>(button)];
}

auto Mouse::IsButtonPressed(const MouseButtonCode& button) -> bool
{
	auto& inst = Instance();

	return inst._buttonStates[static_cast<int>(button)] && !inst._prevButtonStates[static_cast<int>(button)];
}

auto Mouse::IsButtonReleased(const MouseButtonCode& button) -> bool
{
	auto& inst = Instance();

	return !inst._buttonStates[static_cast<int>(button)] && inst._prevButtonStates[static_cast<int>(button)];
}

auto Mouse::IsAnyButtonDown() -> bool
{
	auto& inst = Instance();
	return std::ranges::any_of(inst._buttonStates, [](bool element) { return element; });
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

auto Mouse::VerticalScroll() -> float
{
	auto& inst = Instance();

	return inst._verticalScrollBuffer;
}

auto Mouse::HorizontalScroll() -> float
{
	auto& inst = Instance();

	return inst._horizontalScrollBuffer;
}

void Mouse::OnButtonPress(const MouseButtonEvent& event)
{
	auto& inst = Instance();

	inst._buttonStates[static_cast<int>(event.Button)] = true;
}

void Mouse::OnButtonRelease(const MouseButtonEvent& event)
{
	auto& inst = Instance();

	inst._buttonStates[static_cast<int>(event.Button)] = false;
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

	if (event.Wheel == MouseWheelCode::HorizontalWheel)
	{
		inst._horizontalScrollBuffer += event.Delta;
	}
	else if (event.Wheel == MouseWheelCode::VerticalWheel)
	{
		inst._verticalScrollBuffer += event.Delta;
	}
}
}
