#pragma once

namespace Se
{
struct RenderChannel
{
	static RenderChannel Main;
	static RenderChannel Shadow;

	Uint64 Value;

	RenderChannel(Uint64 value) :
		Value(value)
	{
	}

	bool operator==(const RenderChannel& rhs) const
	{
		return Value == rhs.Value;
	}
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
