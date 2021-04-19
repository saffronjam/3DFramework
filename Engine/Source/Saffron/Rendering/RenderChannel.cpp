#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderChannel.h"

namespace Se
{
RenderChannel RenderChannel::Main(BIT(1));
RenderChannel RenderChannel::Shadow(BIT(2));
RenderChannel RenderChannel::Outline(BIT(3));

RenderChannel::RenderChannel(Uint64 value):
	Value(value)
{
}

Uint64 RenderChannel::operator&(const RenderChannel& rhs) const
{
	return Value & rhs.Value;
}

Uint64 RenderChannel::operator|(const RenderChannel& rhs) const
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
