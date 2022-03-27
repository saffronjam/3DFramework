#pragma once

#include <memory>
#include <Windows.h>

#include "Saffron/Debug/Debug.h"
#include "Saffron/Common/App.h"
#include "Saffron/ErrorHandling/SaffronException.h"

#ifdef SE_PLATFORM_WINDOWS

extern std::unique_ptr<Se::App> Se::CreateApp();

int main(int argc, char** argv)
{
	try
	{
		auto app = Se::CreateApp();
		Se::Debug::Assert(app.get(), "Client App is null!");
		app->Run();
	}
	catch (const Se::SaffronException& e)
	{
		MessageBoxA(nullptr, e.What().c_str(), e.Type(), MB_OK | MB_ICONEXCLAMATION);
	}
}

#endif
