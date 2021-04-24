#include "SaffronPCH.h"

#include "Saffron/Core/Core.h"

constexpr auto BUILD_ID = "v0.1a";

namespace Se
{
void Core::Initialize()
{
	Log::Init();

	Log::CoreTrace("Saffron Engine {}", BUILD_ID);
	Log::CoreTrace("Initializing...");
}

void Core::Shutdown()
{
	Log::CoreTrace("Shutting down...");
}
}
