#pragma once

#include "Saffron/Base.h"

namespace Se
{
struct RenderChannel
{
	static RenderChannel Main;
	static RenderChannel Shadow;
	static RenderChannel Outline;

	Uint64 Value;

	RenderChannel(Uint64 value);

	Uint64 operator &(const RenderChannel& rhs) const;
	Uint64 operator |(const RenderChannel& rhs) const;
	bool operator==(const RenderChannel& rhs) const;
	bool operator!=(const RenderChannel& rhs) const;
};
}

namespace std
{
template <>
struct hash<Se::RenderChannel>
{
	std::size_t operator()(const Se::RenderChannel& renderChannel) const noexcept
	{
		return hash<Se::Uint64>()(renderChannel.Value);
	}
};
}
