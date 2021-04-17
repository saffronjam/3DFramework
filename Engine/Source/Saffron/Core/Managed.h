#pragma once

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
//template <class ManagedType>
//class Managed : public std::enable_shared_from_this<ManagedType>
//{
//public:
//	virtual ~Managed() = default;
//
//	_NODISCARD Shared<ManagedType> GetShared()
//	{
//		return this->shared_from_this();
//	}
//
//	_NODISCARD Shared<const ManagedType> GetShared() const
//	{
//		return this->shared_from_this();
//	}
//
//	template<class DynType>
//	_NODISCARD Shared<DynType> GetDynShared()
//	{
//		return std::dynamic_pointer_cast<DynType>(GetShared());
//	}
//
//	template<class DynType>
//	_NODISCARD Shared<const DynType> GetDynShared() const
//	{
//		return std::dynamic_pointer_cast<const DynType>(GetShared());
//	}
//
//	template<class DynType>
//	_NODISCARD Shared<DynType> GetStaticShared()
//	{
//		return std::static_pointer_cast<DynType>(GetShared());
//	}
//
//	template<class DynType>
//	_NODISCARD Shared<const DynType> GetStaticShared() const
//	{
//		return std::static_pointer_cast<const DynType>(GetShared());
//	}
//
//	_NODISCARD Weak<ManagedType> GetWeak() noexcept
//	{
//		return this->weak_from_this();
//	}
//
//	_NODISCARD Weak<const ManagedType> GetWeak() const noexcept
//	{
//		return this->weak_from_this();
//	}
//};

class Managed
{
public:
	virtual ~Managed() = default;

	void IncRefCount() const
	{
		_refCount++;
	}

	void DecRefCount() const
	{
		_refCount--;
	}

	uint32_t GetRefCount() const { return _refCount; }

private:
	mutable uint32_t _refCount = 0; // TODO: atomic
};
}
