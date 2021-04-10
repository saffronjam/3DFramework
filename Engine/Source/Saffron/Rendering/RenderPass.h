#pragma once

#include "Saffron/Base.h"

namespace Se
{
class RenderPass
{
public:
	virtual ~RenderPass() = default;

	virtual void Execute() = 0;
};
}
