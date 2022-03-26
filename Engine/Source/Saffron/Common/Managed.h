#pragma once

#include <memory>

namespace Se
{
template <class T>
class Managed : public std::enable_shared_from_this<T>
{
public:
	[[nodiscard]] std::weak_ptr<T> BorrowThis()
	{
		return this->weak_from_this();
	}

	[[nodiscard]] std::weak_ptr<const T> BorrowThis() const
	{
		return this->weak_from_this();
	}

	[[nodiscard]] std::shared_ptr<T> ShareThis()
	{
		return this->shared_from_this();
	}

	[[nodiscard]] std::shared_ptr<const T> ShareThis() const
	{
		return this->shared_from_this();
	}

	template <typename U>
	[[nodiscard]] std::shared_ptr<U> ShareThisAs()
	{
		return std::dynamic_pointer_cast<U>(this->shared_from_this());
	}

	template <typename U>
	[[nodiscard]] std::shared_ptr<const U> ShareThisAs() const
	{
		return std::dynamic_pointer_cast<U>(this->shared_from_this());
	}
};
}
