#pragma once

#undef UUID_DEFINED

#include <xhash>

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
class Uuid
{
public:
	Uuid();
	Uuid(ulong uuid);
	Uuid(const Uuid& other);

	operator ulong() { return _uuid; }

	operator const ulong() const { return _uuid; }

	template<typename Comparable>
	auto operator==(const Comparable& other) const -> bool
	{
		return _uuid == static_cast<const ulong&>(other);
	}

	static auto Null() -> Uuid { return {0ull}; }

private:
	ulong _uuid;
};
}

namespace std
{
template <>
struct hash<Se::Uuid>
{
	auto operator()(const Se::Uuid& uuid) const -> std::size_t
	{
		return hash<Se::ulong>()(static_cast<Se::ulong>(uuid));
	}
};
}
