#include "SaffronPCH.h"

#include "Saffron/Common/SubscriberList.h"

namespace Se
{
auto SubscriberList<void>::operator+=(const Event& event) -> Uuid
{
	return Subscribe(event);
}

auto SubscriberList<void>::operator+=(const Action& action) -> Uuid
{
	return Subscribe(action);
}

void SubscriberList<void>::operator-=(Uuid uuid)
{
	return Unsubscribe(uuid);
}

void SubscriberList<void>::Invoke()
{
	if (!_subscribers.has_value())
	{
		return;
	}
	
	for (auto& subscriber : *_subscribers | std::views::values)
	{
		if (subscriber())
		{
			break;
		}
	}
}

auto SubscriberList<void>::Subscribe(const Event& event) -> Uuid
{
	if (!_subscribers.has_value())
	{
		_subscribers = std::make_optional<std::map<Uuid, Event>>();
	}

	const Uuid uuid;
	_subscribers->emplace(uuid, event);
	return uuid;
}

auto SubscriberList<void>::Subscribe(const Action& action) -> Uuid
{
	if (!_subscribers.has_value())
	{
		_subscribers = std::make_optional<std::map<Uuid, Event>>();
	}

	const Uuid uuid;
	_subscribers->emplace(uuid, [action]()
	{
		action();
		return false;
	});
	return uuid;
}

void SubscriberList<void>::Unsubscribe(Uuid uuid)
{
	const auto result = _subscribers->find(uuid);
	if (result != _subscribers->end())
	{
		_subscribers->erase(result);
	}
}
}
