#pragma once

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
template <class ManagedType>
class MemManaged : public std::enable_shared_from_this<ManagedType>
{
public:
	virtual ~MemManaged() = default;

	_NODISCARD std::shared_ptr<ManagedType> GetShared()
	{
		return this->shared_from_this();
	}

	_NODISCARD std::shared_ptr<const ManagedType> GetShared() const
	{
		return this->shared_from_this();
	}

	template<class DynType>
	_NODISCARD std::shared_ptr<DynType> GetDynShared()
	{
		return std::dynamic_pointer_cast<DynType>(GetShared());
	}

	template<class DynType>
	_NODISCARD std::shared_ptr<const DynType> GetDynShared() const
	{
		return std::dynamic_pointer_cast<const DynType>(GetShared());
	}

	template<class DynType>
	_NODISCARD std::shared_ptr<DynType> GetStaticShared()
	{
		return std::static_pointer_cast<DynType>(GetShared());
	}

	template<class DynType>
	_NODISCARD std::shared_ptr<const DynType> GetStaticShared() const
	{
		return std::static_pointer_cast<const DynType>(GetShared());
	}

	_NODISCARD Weak<ManagedType> GetWeak() noexcept
	{
		return this->weak_from_this();
	}

	_NODISCARD Weak<const ManagedType> GetWeak() const noexcept
	{
		return this->weak_from_this();
	}
};

}