#pragma once

#include "Saffron/Core/Managed.h"

namespace Se
{
class Resource : public Managed
{
public:
	template <typename T>
	using Ref = Shared<T>;

	virtual ~Resource() = default;

	virtual size_t GetResourceID() { return 0; }
};
}
