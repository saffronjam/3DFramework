#pragma once

#include "Saffron/Config.h"

namespace Se
{
class GraphicsContext
{
public:
	virtual ~GraphicsContext() = default;

	virtual void SwapBuffers() = 0;

	static Scope<GraphicsContext> Create(void *window);
};
}
