#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/DrawCommand.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class RenderCommandPass : public RenderPass
{
public:
	void Execute() override;

protected:
	void Add(const RenderCommand &command);

private:
	ArrayList<RenderCommand> _commands;
};
}
