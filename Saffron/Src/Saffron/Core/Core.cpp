#include "Saffron/SaffronPCH.h"

#include "Saffron/Core/Core.h"

constexpr auto BUILD_ID = "v0.1a";

namespace Se
{

void Core::Initialize()
{
	Log::Init();

	SE_CORE_TRACE("Saffron Engine {}", BUILD_ID);
	SE_CORE_TRACE("Initializing...");
}

void Core::Shutdown()
{
	SE_CORE_TRACE("Shutting down...");
}

}
