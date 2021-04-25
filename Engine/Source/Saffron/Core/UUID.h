#pragma once

#undef UUID_DEFINED

#include <xhash>

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
class UUID
{
public:
	UUID();
	UUID(ulong uuid);
	UUID(const UUID& other);

	operator ulong() { return _uuid; }

	operator const ulong() const { return _uuid; }

	bool operator==(const UUID& other) const
	{
		return _uuid == other._uuid;
	}

	static UUID Null() { return UUID(0ull); }

private:
	ulong _uuid;
};
}

namespace std
{
template <>
struct hash<Se::UUID>
{
	std::size_t operator()(const Se::UUID& uuid) const
	{
		return hash<Se::ulong>()(static_cast<Se::ulong>(uuid));
	}
};
}
