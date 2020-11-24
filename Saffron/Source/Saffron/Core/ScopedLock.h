#pragma once

#include <mutex>

namespace Se
{
template<class... MutexTypes>
class ScopedLock
{
public:
	explicit ScopedLock(MutexTypes&... mutexes)
		: m_Mutexes(mutexes)
	{
		std::lock(m_Mutexes);
	}

	~ScopedLock() noexcept
	{
		std::apply([](MutexTypes&... mutexTypes) {(..., (void)mutexTypes.unlock()); }, m_Mutexes);
	}

	ScopedLock(const ScopedLock &) = delete;
	ScopedLock &operator=(const ScopedLock &) = delete;

private:
	Tuple<MutexTypes&...> m_Mutexes;
};

template<class MutexType>
class ScopedLock<MutexType>
{
public:
	explicit ScopedLock(MutexType &mutex)
		: m_Mutex(mutex)
	{
		m_Mutex.lock();
	}

	~ScopedLock() noexcept
	{
		m_Mutex.unlock();
	}

	ScopedLock(const ScopedLock &) = delete;
	ScopedLock &operator=(const ScopedLock &) = delete;

private:
	MutexType &m_Mutex;
};
}