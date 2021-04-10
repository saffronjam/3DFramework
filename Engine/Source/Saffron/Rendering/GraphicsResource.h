#pragma once

#include <bgfx/bgfx.h>

#include "Saffron/Base.h"

namespace Se
{
class GraphicsResource : MemManaged<GraphicsResource>
{
public:	
	virtual ~GraphicsResource() = default;
};
}
