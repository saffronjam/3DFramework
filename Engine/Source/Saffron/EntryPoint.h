#pragma once


#include "Saffron/Core/Core.h"
#include "Saffron/Debug/Debug.h"

#ifdef SE_PLATFORM_WINDOWS

extern Se::App* Se::CreateApplication();

int main(int argc, char** argv)
{
	Se::Core::Initialize();
	Se::App* app = Se::CreateApplication();
	Se::Debug::Assert(app, "Client App is null!");;
	app->Run();
	delete app;
	Se::Core::Shutdown();
}

#endif
