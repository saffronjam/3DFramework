#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Event/MouseEvent.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/System/SeMath.h"

namespace Saffron
{
class SAFFRON_API Mouse : public EventHandler
{
public:
	using Button = ButtonCode;

public:
	explicit Mouse(class Window &window);
	virtual ~Mouse() = default;

	void Update() noexcept;
	void OnEvent(const Event::Ptr pEvent) override;

	bool IsDown(Button button) const noexcept;
	bool WasDown(Button button) const noexcept;
	bool IsPressed(Button button) const noexcept;
	bool IsReleased(Button button) const noexcept;
	bool IsAnyDown() const noexcept;
	bool IsInScreen() const noexcept;

	const glm::vec2 &GetPosition() const noexcept;
	const glm::vec2 &GetDelta() const noexcept;

private:
	void OnPress(const MousePressEvent::Ptr event);
	void OnRelease(const MouseReleaseEvent::Ptr event);
	void OnMove(const MouseMoveEvent::Ptr event);
	void OnEnter(const MouseEnterEvent::Ptr event);
	void OnLeave(const MouseLeaveEvent::Ptr event);

private:
	class Window &m_wnd;

	mutable std::map<Button, bool> m_buttonmap;
	mutable std::map<Button, bool> m_prevButtonmap;

	glm::vec2 m_position;
	glm::vec2 m_moveDelta;

	bool m_isInScreen;
};
}