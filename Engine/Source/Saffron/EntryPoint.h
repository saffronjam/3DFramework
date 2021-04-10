#pragma once

#include "Saffron/Common/Core.h"

#ifdef SE_PLATFORM_WINDOWS

extern Se::App* Se::CreateApplication();

int main(int _argc, char** _argv)
{
	Se::Core::Initialize();
	Se::App* app = Se::CreateApplication();
	SE_CORE_ASSERT(app, "Client App is null!");
	app->Run();
	delete app;
	Se::Core::Shutdown();
	return 0;
};

#endif
