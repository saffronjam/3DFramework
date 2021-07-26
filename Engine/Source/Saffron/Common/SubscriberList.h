#pragma once

#include <functional>
#include <optional>
#include <map>
#include <ranges>

#include "Saffron/Common/Uuid.h"

namespace Se
{
template <typename ... Args>
class SubscriberList
{
	using Event = std::function<bool(Args ...)>;
	using Action = std::function<void(Args ...)>;

public:
	auto operator +=(const Event& event) -> Uuid;
	auto operator +=(const Action& action) -> Uuid;
	void operator -=(Uuid uuid);

	void Invoke(Args ... args);

	auto Subscribe(const Event& event) -> Uuid;
	auto Subscribe(const Action& action) -> Uuid;
	void Unsubscribe(Uuid uuid);

private:
	std::optional<std::map<Uuid, Event>> _subscribers;
};

template <>
class SubscriberList<void>
{
	using Event = std::function<bool()>;
	using Action = std::function<void()>;

public:
	auto operator +=(const Event& event) -> Uuid;
	auto operator +=(const Action& action) -> Uuid;
	void operator -=(Uuid uuid);

	void Invoke();

	auto Subscribe(const Event& event) -> Uuid;
	auto Subscribe(const Action& action) -> Uuid;
	void Unsubscribe(Uuid uuid);

private:
	std::optional<std::map<Uuid, Event>> _subscribers;
};

template <typename ... Args>
auto SubscriberList<Args...>::operator+=(const Event& event) -> Uuid
{
	return Subscribe(event);
}

template <typename ... Args>
auto SubscriberList<Args...>::operator+=(const Action& action) -> Uuid
{
	return Subscribe(action);
}

template <typename ... Args>
void SubscriberList<Args...>::operator-=(Uuid uuid)
{
	Unsubscribe(uuid);
}

template <typename ... Args>
void SubscriberList<Args...>::Invoke(Args ... args)
{
	if (!_subscribers.has_value())
	{
		return;
	}

	for (auto& subscriber : *_subscribers | std::views::values)
	{
		if (subscriber(std::forward(args...)))
		{
			break;
		}
	}
}

template <typename ... Args>
Uuid SubscriberList<Args...>::Subscribe(const Event& event)
{
	if (!_subscribers.has_value())
	{
		_subscribers = std::make_optional<std::map<Uuid, Event>>();
	}

	const Uuid uuid;
	_subscribers->emplace(uuid, event);
	return uuid;
}

template <typename ... Args>
Uuid SubscriberList<Args...>::Subscribe(const Action& action)
{
	if (!_subscribers.has_value())
	{
		_subscribers = std::make_optional<decltype(_subscribers)>();
	}

	const Uuid uuid;
	_subscribers->emplace(uuid, [action](Args ...)
	{
		action();
		return false;
	});
	return uuid;
}

template <typename ... Args>
void SubscriberList<Args...>::Unsubscribe(Uuid uuid)
{
	const auto result = _subscribers->find(uuid);
	if (result != _subscribers->end())
	{
		_subscribers->erase(result);
	}
}
}
