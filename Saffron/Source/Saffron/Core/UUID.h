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
	UUID(Uint64 uuid);
	UUID(const UUID &other);

	operator Uint64 () { return m_UUID; }
	operator const Uint64() const { return m_UUID; }

	bool operator==(const UUID &other) const
	{
		return m_UUID == other.m_UUID;
	}

	static UUID Null() { return UUID(0ull); }

private:
	Uint64 m_UUID;
};
}

namespace std
{
template <>
struct hash<Se::UUID>
{
	std::size_t operator()(const Se::UUID &uuid) const
	{
		return hash<Se::Uint64>()(static_cast<Se::Uint64>(uuid));
	}
};
}

