#include "SaffronPCH.h"

#include "Saffron/Core/EventSubscriberList.h"

namespace Se
{
void EventSubscriberList<void>::Invoke()
{
	for (const auto& [token, subscriber] : _subscribers)
	{
		if (subscriber())
		{
			break;
		}
	}
}

EventSubscriberList<void>::CancellationToken EventSubscriberList<void>::Subscribe(Handler handler)
{
	const CancellationToken token;
	_subscribers.emplace(token, Move(handler));
	return token;
}

void EventSubscriberList<void>::Unsubscribe(CancellationToken token)
{
	_subscribers.erase(token);
}

EventSubscriberList<void>::CancellationToken EventSubscriberList<void>::operator+=(Handler handler)
{
	return Subscribe(handler);
}

void EventSubscriberList<void>::operator-=(CancellationToken token)
{
	Unsubscribe(token);
}
}
