#pragma once

#include "Saffron/Config.h"
#include "Saffron/Core/Core.h"

#ifdef SE_PLATFORM_WINDOWS


extern Se::Ref<Se::Application> CreateApplication();

int main(int argc, char **argv)
{
	Se::Core::Initialize();
	Se::Ref<Se::Application> app = CreateApplication();
	SE_CORE_ASSERT(app, "Client Application is null!");
	app->Run();
	Se::Core::Shutdown();
}

#endif

