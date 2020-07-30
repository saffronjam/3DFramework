#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Event/KeyboardEvent.h"

namespace Se
{
class Keyboard
{
public:
	using Key = KeyCode;

public:
	Keyboard() = default;
	virtual ~Keyboard() = default;

	void OnUpdate();
	void OnEvent(const Event &event);

	bool IsDown(Key key) const;
	bool WasDown(Key key) const;
	bool IsPressed(Key key) const;
	bool IsReleased(Key key) const;
	bool IsBeingRepeated(Key key) const;
	bool IsAnyDown() const;

private:
	void OnPress(const KeyboardPressEvent &event);
	void OnRelease(const KeyboardReleaseEvent &event);
	void OnRepeat(const KeyboardRepeatEvent &event);

private:
	mutable std::map<Key, bool> m_keymap;
	mutable std::map<Key, bool> m_prevKeymap;
	mutable std::map<Key, bool> m_repeatKeymap;
};
}
