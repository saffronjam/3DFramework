#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Event/MouseEvent.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Mouse
{
public:
	using Button = ButtonCode;

public:
	Mouse();
	virtual ~Mouse() = default;

	void OnUpdate();
	void OnEvent(const Event &event);

	bool IsDown(Button button) const;
	bool WasDown(Button button) const;
	bool IsPressed(Button button) const;
	bool IsReleased(Button button) const;
	bool IsAnyDown() const;
	bool IsInScreen() const;

	const glm::vec2 &GetPosition() const;
	const glm::vec2 &GetDelta() const;
	float GetHScroll() const;
	float GetVScroll() const;

private:
	void OnPress(const MousePressEvent &event);
	void OnRelease(const MouseReleaseEvent &event);
	void OnScroll(const MouseScrollEvent &event);
	void OnMove(const MouseMoveEvent &event);
	void OnEnter(const MouseEnterEvent &event);
	void OnLeave(const MouseLeaveEvent &event);

private:
	mutable std::map<Button, bool> m_ButtonMap;
	mutable std::map<Button, bool> m_PrevButtonMap;

	glm::vec2 m_Position;
	glm::vec2 m_MoveDelta;
	glm::vec2 m_Scroll;

	bool m_IsInScreen;
};
}
