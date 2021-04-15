#pragma once

#include "Saffron/Core/UUID.h"
#include "Saffron/Core/TypeDefs.h"

namespace Se
{
template <typename EventArgs>
class EventSubscriberList
{
public:
	using CancellationToken = UUID;
	using Handler = Function<bool(EventArgs)>;

public:
	void Invoke(EventArgs args);

	CancellationToken Subscribe(Handler handler);
	void Unsubscribe(CancellationToken token);

	CancellationToken operator +=(Handler handler);
	void operator -=(CancellationToken token);

private:
	UnorderedMap<CancellationToken, Handler> _subscribers;
};

template <>
class EventSubscriberList<void>
{
public:
	using CancellationToken = UUID;
	using Handler = Function<bool()>;

public:
	void Invoke();

	CancellationToken Subscribe(Handler handler);
	void Unsubscribe(CancellationToken token);

	CancellationToken operator +=(Handler handler);
	void operator -=(CancellationToken token);

private:
	UnorderedMap<CancellationToken, Handler> _subscribers;
};

template <typename EventArgs>
void EventSubscriberList<EventArgs>::Invoke(EventArgs args)
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
	_subscribers.erase(token);
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
