#pragma once

#include <mutex>

namespace Se
{
template <class... MutexTypes>
class ScopedLock
{
public:
	explicit ScopedLock(MutexTypes&... mutexes) :
		_mutexes(mutexes)
	{
		std::lock(_mutexes);
	}

	~ScopedLock() noexcept
	{
		std::apply([](MutexTypes&... mutexTypes) { (..., (void)mutexTypes.unlock()); }, _mutexes);
	}

	ScopedLock(const ScopedLock&) = delete;
	ScopedLock& operator=(const ScopedLock&) = delete;

private:
	Tuple<MutexTypes&...> _mutexes;
};

template <class MutexType>
class ScopedLock<MutexType>
{
public:
	explicit ScopedLock(MutexType& mutex) :
		_mutex(mutex)
	{
		_mutex.lock();
	}

	~ScopedLock() noexcept
	{
		_mutex.unlock();
	}

	ScopedLock(const ScopedLock&) = delete;
	ScopedLock& operator=(const ScopedLock&) = delete;

private:
	MutexType& _mutex;
};
}
