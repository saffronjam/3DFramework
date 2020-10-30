#pragma once

#include <atomic>

#include "Saffron/System/TypeDefs.h"

namespace Se
{

class ReferenceCounted
{
public:
	virtual ~ReferenceCounted() = default;

	void IncreaseReferenceCount() const
	{
		++m_ReferenceCount;
	}
	void DecreaseReferenceCount() const
	{
		--m_ReferenceCount;
	}

	Uint32 GetReferenceCount() const { return m_ReferenceCount; }
private:
	mutable std::atomic<Uint32> m_ReferenceCount = 0;
};

template<typename SharedType>
class Shared
{
public:
	Shared()
		: m_Instance(nullptr)
	{
	}

	Shared(std::nullptr_t n)
		: m_Instance(nullptr)
	{
	}

	Shared(SharedType *instance)
		: m_Instance(instance)
	{
		static_assert(std::is_base_of<ReferenceCounted, SharedType>::value, "Class is not ReferenceCounted!");
		IncreaseReferenceCount();
	}

	template<typename SharedType2>
	Shared(const Shared<SharedType2> &other)
	{
		m_Instance = static_cast<SharedType *>(other.m_Instance);
		IncreaseReferenceCount();
	}

	template<typename SharedType2>
	Shared(Shared<SharedType2> &&other)
	{
		m_Instance = static_cast<SharedType *>(other.m_Instance);
		other.m_Instance = nullptr;
	}

	~Shared()
	{
		DecreaseReferenceCount();
	}

	Shared(const Shared<SharedType> &other)
		: m_Instance(other.m_Instance)
	{
		IncreaseReferenceCount();
	}

	Shared &operator=(std::nullptr_t)
	{
		DecreaseReferenceCount();
		m_Instance = nullptr;
		return *this;
	}

	Shared &operator=(const Shared<SharedType> &other)
	{
		other.IncreaseReferenceCount();
		DecreaseReferenceCount();

		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename SharedType2>
	Shared &operator=(const Shared<SharedType2> &other)
	{
		other.IncreaseReferenceCount();
		DecreaseReferenceCount();

		m_Instance = dynamic_cast<SharedType>(other.m_Instance);

		SE_CORE_ASSERT(m_Instance, "Shared pointer type mismatch");

		return *this;
	}

	template<typename SharedType2>
	Shared &operator=(Shared<SharedType2> &&other)
	{
		DecreaseReferenceCount();

		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }
	operator bool() const { return m_Instance != nullptr; }

	SharedType *operator->() { return m_Instance; }
	const SharedType *operator->() const { return m_Instance; }

	SharedType &operator*() { return *m_Instance; }
	const SharedType &operator*() const { return *m_Instance; }

	SharedType *Raw() { return  m_Instance; }
	const SharedType *Raw() const { return  m_Instance; }

	void Reset(SharedType *instance = nullptr)
	{
		DecreaseReferenceCount();
		m_Instance = instance;
		IncreaseReferenceCount();
	}

	template<typename... Args>
	static Shared<SharedType> Create(Args&&... args)
	{
		return Shared<SharedType>(new SharedType(std::forward<Args>(args)...));
	}


	template <class SharedType2>
	static Shared<SharedType> Cast(const Shared<SharedType2> &other) noexcept
	{
		// dynamic_cast for Shared that properly respects the reference count control block
		const auto shared = dynamic_cast<const SharedType *>(other.Raw());

		if ( shared )
		{
			return Shared<SharedType>(other);
		}

		return nullptr;
	}

private:
	void IncreaseReferenceCount() const
	{
		if ( m_Instance )
		{
			m_Instance->IncreaseReferenceCount();
		}
	}

	void DecreaseReferenceCount() const
	{
		if ( m_Instance )
		{
			m_Instance->DecreaseReferenceCount();
			if ( m_Instance->GetReferenceCount() == 0 )
			{
				delete m_Instance;
			}
		}
	}

	template<class SharedType2>
	friend class Shared;
	SharedType *m_Instance;
};

template<typename WeakType>
class Weak
{
public:
	Weak()
		: m_Instance(nullptr)
	{
	}

	Weak(std::nullptr_t n)
		: m_Instance(nullptr)
	{
	}

	Weak(WeakType *instance)
		: m_Instance(instance)
	{
		static_assert(std::is_base_of<ReferenceCounted, WeakType>::value, "Class is not ReferenceCounted!");
	}

	template<typename WeakType2>
	Weak(const Weak<WeakType2> &other)
		: m_Instance(static_cast<WeakType *>(other.m_Instance))
	{
	}

	template<typename WeakType2>
	Weak(Weak<WeakType2> &&other)
	{
		m_Instance = static_cast<WeakType *>(other.m_Instance);
		other.m_Instance = nullptr;
	}

	~Weak()
	{
	}

	Weak(const Weak<WeakType> &other)
		: m_Instance(other.m_Instance)
	{
	}

	Weak &operator=(std::nullptr_t)
	{
		m_Instance = nullptr;
		return *this;
	}

	Weak &operator=(const Weak<WeakType> &other)
	{
		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename WeakType2>
	Weak &operator=(const Weak<WeakType2> &other)
	{
		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename WeakType2>
	Weak &operator=(Weak<WeakType2> &&other)
	{
		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }
	operator bool() const { return m_Instance != nullptr; }

	WeakType *operator->() { return m_Instance; }
	const WeakType *operator->() const { return m_Instance; }

	WeakType &operator*() { return *m_Instance; }
	const WeakType &operator*() const { return *m_Instance; }

	WeakType *Raw() { return  m_Instance; }
	const WeakType *Raw() const { return  m_Instance; }

	void Reset(WeakType *instance = nullptr)
	{
		m_Instance = instance;
	}

	template<typename... Args>
	static Weak<WeakType> Create(Args&&... args)
	{
		return Weak<WeakType>(new WeakType(std::forward<Args>(args)...));
	}

	template<class WeakType2>
	friend class Weak;
	WeakType *m_Instance;
};

template<typename UniqueType>
class Unique
{
public:
	Unique()
		: m_Instance(nullptr)
	{
	}

	Unique(std::nullptr_t n)
		: m_Instance(nullptr)
	{
	}

	Unique(UniqueType *instance)
		: m_Instance(instance)
	{
		static_assert(std::is_base_of<ReferenceCounted, UniqueType>::value, "Class is not ReferenceCounted!");
		Set();
	}

	template<typename UniqueType2>
	Unique(const Unique<UniqueType2> &other) = delete;

	template<typename UniqueType2>
	Unique(Unique<UniqueType2> &&other)
	{
		m_Instance = static_cast<UniqueType *>(other.m_Instance);
		other.m_Instance = nullptr;
	}

	~Unique()
	{
		Unset();
	}

	Unique(const Unique<UniqueType> &other) = delete;

	Unique &operator=(std::nullptr_t)
	{
		Unset();
		m_Instance = nullptr;
		return *this;
	}

	Unique &operator=(const Unique<UniqueType> &other) = delete;

	template<typename UniqueType2>
	Unique &operator=(const Unique<UniqueType2> &other) = delete;

	template<typename UniqueType2>
	Unique &operator=(Unique<UniqueType2> &&other)
	{
		Unset();
		m_Instance = other.m_Instance;
		other.m_Instance = nullptr;
		return *this;
	}

	operator bool() { return m_Instance != nullptr; }
	operator bool() const { return m_Instance != nullptr; }

	UniqueType *operator->() { return m_Instance; }
	const UniqueType *operator->() const { return m_Instance; }

	UniqueType &operator*() { return *m_Instance; }
	const UniqueType &operator*() const { return *m_Instance; }

	UniqueType *Raw() { return  m_Instance; }
	const UniqueType *Raw() const { return  m_Instance; }

	void Reset(UniqueType *instance = nullptr)
	{
		Unset();
		m_Instance = instance;
		Set();
	}

	template<typename... Args>
	static Unique<UniqueType> Create(Args&&... args)
	{
		return Unique<UniqueType>(new UniqueType(std::forward<Args>(args)...));
	}

private:
	void Set() const
	{
		if ( m_Instance )
		{
			m_Instance->IncreaseReferenceCount();
		}
	}

	void Unset() const
	{
		if ( m_Instance )
		{
			m_Instance->DecreaseReferenceCount();
			if ( m_Instance->GetReferenceCount() == 0 )
			{
				delete m_Instance;
			}
		}
	}

	template<class UniqueType2>
	friend class Unique;
	UniqueType *m_Instance;
};

}