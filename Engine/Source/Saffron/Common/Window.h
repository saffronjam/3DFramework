#pragma once

#include "Saffron/Base.h"
#include "Saffron/Input/Codes.h"
#include "Saffron/Event/Event.h"

namespace Se
{
struct WindowSpec
{
	uint Width = 1280;
	uint Height = 720;
	std::string Title;
};

class Window
{
public:
	explicit Window(const WindowSpec& spec);

	void PushEvent(const Event& event);
	void DispatchEvents();

	static auto Create(const WindowSpec& spec) -> std::unique_ptr<Window>;

public:
	SubscriberList<void> Closed;
	SubscriberList<void> Resized;
	SubscriberList<void> LostFocus;
	SubscriberList<void> GainedFocus;

protected:
	WindowSpec _spec;

private:
	std::vector<Event> _events;
};
}
