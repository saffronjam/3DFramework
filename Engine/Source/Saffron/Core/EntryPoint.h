#pragma once

#include <memory>

#include "Saffron/Debug/Debug.h"

#ifdef SE_PLATFORM_WINDOWS

extern std::unique_ptr<Se::App> Se::CreateApp();

int main(int argc, char** argv)
{
	std::unique_ptr<Se::App> app = Se::CreateApp();
	Se::Debug::Assert(app.get(), "Client App is null!");;
	app->Run();
}

#endif
