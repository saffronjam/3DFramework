#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Event/KeyboardEvent.h"
#include "Saffron/Event/EventHandler.h"

namespace Saffron
{
class SAFFRON_API Keyboard : public EventHandler
{
public:
	using Key = KeyCode;

public:
	explicit Keyboard(class Window &window);
	virtual ~Keyboard() = default;

	void Update() noexcept;
	void OnEvent(const Event::Ptr pEvent) override;

	bool IsDown(Key key) const noexcept;;
	bool WasDown(Key key) const noexcept;;
	bool IsPressed(Key key) const noexcept;;
	bool IsReleased(Key key) const noexcept;;
	bool IsBeingRepeated(Key key) const noexcept;;
	bool IsAnyDown() const noexcept;;

private:
	void OnPress(const KeyboardPressEvent::Ptr event);
	void OnRelease(const KeyboardReleaseEvent::Ptr event);
	void OnRepeat(const KeyboardRepeatEvent::Ptr event);

private:
	class Window &m_wnd;

	mutable std::map<Key, bool> m_keymap;
	mutable std::map<Key, bool> m_prevKeymap;
	mutable std::map<Key, bool> m_repeatKeymap;
};
}
