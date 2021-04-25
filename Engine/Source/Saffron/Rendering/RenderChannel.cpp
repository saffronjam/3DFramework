#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderChannel.h"

namespace Se
{
RenderChannel RenderChannel::Main(BIT(1));
RenderChannel RenderChannel::Shadow(BIT(2));
RenderChannel RenderChannel::Outline(BIT(3));

RenderChannel::RenderChannel(ulong value):
	Value(value)
{
}

ulong RenderChannel::operator&(const RenderChannel& rhs) const
{
	return Value & rhs.Value;
}

ulong RenderChannel::operator|(const RenderChannel& rhs) const
{
	return Value | rhs.Value;
}

bool RenderChannel::operator==(const RenderChannel& rhs) const
{
	return Value == rhs.Value;
}

bool RenderChannel::operator!=(const RenderChannel& rhs) const
{
	return Value != rhs.Value;
}
}
