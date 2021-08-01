#pragma once

#include "Saffron/Base.h"
#include "Saffron/Event/Event.h"

namespace Se
{
struct WindowSpec
{
	uint Width = 1280;
	uint Height = 720;
	std::string Title;
	Vector2 Position;
};

class Window
{
public:
	explicit Window(const WindowSpec& spec);
	virtual ~Window() = default;

	virtual void OnUpdate();

	void PushEvent(const Event& event);
	void DispatchEvents();

	auto Width() const -> uint;
	auto Height() const -> uint;

	virtual auto NativeHandle() const -> void* = 0;

	static auto Create(const WindowSpec& spec) -> std::unique_ptr<Window>;

public:
	// Window events
	mutable SubscriberList<void> Closed;
	mutable SubscriberList<const SizeEvent&> Resized;
	mutable SubscriberList<void> LostFocus;
	mutable SubscriberList<void> GainedFocus;

	// Keyboard events
	mutable SubscriberList<const TextEvent&> TextEntered;
	mutable SubscriberList<const KeyEvent&> KeyPressed;
	mutable SubscriberList<const KeyEvent&> KeyReleased;

	// Mouse events
	mutable SubscriberList<const MouseWheelScrollEvent&> MouseWheelScrolled;
	mutable SubscriberList<const MouseButtonEvent&> MouseButtonPressed;
	mutable SubscriberList<const MouseButtonEvent&> MouseButtonReleased;
	mutable SubscriberList<const MouseMoveEvent&> MouseMoved;
	mutable SubscriberList<void> MouseEntered;
	mutable SubscriberList<void> MouseLeft;

protected:
	WindowSpec _spec;

private:
	std::vector<Event> _events;
};
}
