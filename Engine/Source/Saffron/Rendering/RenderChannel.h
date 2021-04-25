#pragma once

#include "Saffron/Base.h"

namespace Se
{
struct RenderChannel
{
	static RenderChannel Main;
	static RenderChannel Shadow;
	static RenderChannel Outline;

	ulong Value;

	RenderChannel(ulong value);

	ulong operator &(const RenderChannel& rhs) const;
	ulong operator |(const RenderChannel& rhs) const;
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
		return hash<Se::ulong>()(renderChannel.Value);
	}
};
}
