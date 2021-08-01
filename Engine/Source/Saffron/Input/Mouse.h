#pragma once

#include "Saffron/Base.h"
#include "Saffron/Event/Event.h"

namespace Se
{
class Mouse : public SingleTon<Mouse>
{
public:
	Mouse();

	// Move buttonMap into prev-buttonMap
	void OnUpdate();

	static auto IsButtonDown(const MouseButtonCode& button) -> bool;
	static auto IsButtonPressed(const MouseButtonCode& button) -> bool;
	static auto IsButtonReleased(const MouseButtonCode& button) -> bool;
	static auto IsAnyButtonDown() -> bool;

	static auto IsCursorEnabled() -> bool;
	static auto IsCursorInWindow() -> bool;

	static auto CursorPosition(bool normalized = false) -> const Vector2&;
	static auto Swipe() -> const Vector2&;

	static auto VerticalScroll() -> float;
	static auto HorizontalScroll() -> float;

private:
	static void OnButtonPress(const MouseButtonEvent& event);
	static void OnButtonRelease(const MouseButtonEvent& event);
	static void OnMove(const MouseMoveEvent& event);
	static void OnCursorEnter();
	static void OnCursorLeave();
	static void OnScroll(const MouseWheelScrollEvent& event);
	
private:
	std::array<bool, static_cast<int>(MouseButtonCode::Count)> _buttonStates;
	std::array<bool, static_cast<int>(MouseButtonCode::Count)> _prevButtonStates;

	Vector2 _mousePosition = Vector2(0.0f, 0.0f);
	Vector2 _mousePositionNDC = Vector2(0.0f, 0.0f);
	bool _inWindow = true;

	Vector2 _mouseSwipe = Vector2(0.0f, 0.0f);

	float _verticalScrollBuffer = 0.0f;
	float _horizontalScrollBuffer = 0.0f;

	bool _cursorEnabled = true;
	bool _cursorInWindow = true;
};
}
