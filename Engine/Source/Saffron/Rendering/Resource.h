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

	virtual ulong GetResourceID() { return 0; }

protected:
	static ulong HashFilepath(const Filepath& filepath);
	static ulong HashString(const String& string);
};
}
