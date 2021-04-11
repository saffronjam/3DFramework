#pragma once

#include "Saffron/Base.h"

namespace Se
{
template <typename EventArgs>
class EventSubscriberList
{
public:
	using CancellationToken = UUID;
	using Handler = Function<bool(const EventArgs&)>;

public:
	void Invoke(const EventArgs &args);

	CancellationToken Subscribe(Handler handler);
	void Unsubscribe(CancellationToken token);

	CancellationToken operator +=(Handler handler);
	void operator -=(CancellationToken token);

private:
	UnorderedMap<CancellationToken, Handler> _subscribers;
};

template <typename EventArgs>
void EventSubscriberList<EventArgs>::Invoke(const EventArgs &args)
{
	for (const auto& [token, subscriber] : _subscribers)
	{
		if (subscriber(args))
		{
			break;
		}
	}
}

template <typename EventArgs>
typename EventSubscriberList<EventArgs>::CancellationToken EventSubscriberList<EventArgs>::Subscribe(Handler handler)
{
	const CancellationToken token;
	_subscribers.emplace(token, Move(handler));
	return token;
}

template <typename EventArgs>
void EventSubscriberList<EventArgs>::Unsubscribe(CancellationToken token)
{
	const auto result = _subscribers.find(token);
	if (result != _subscribers.end())
	{
		_subscribers.erase(result);
	}
}

template <typename EventArgs>
typename EventSubscriberList<EventArgs>::CancellationToken EventSubscriberList<EventArgs>::operator+=(Handler handler)
{
	return Subscribe(handler);
}

template <typename EventArgs>
void EventSubscriberList<EventArgs>::operator-=(CancellationToken token)
{
	Unsubscribe(token);
}
}
