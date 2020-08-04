#pragma once

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Engine.h"

#ifdef SE_PLATFORM_WINDOWS

extern Se::Ref<Se::Application> Se::CreateApplication();

int main()
{
	Se::Engine::Initialize(Se::Engine::Subsystem::Graphics);
	Se::Log::Init();

	SE_PROFILE_BEGIN_SESSION("Startup", "SaffronProfile-Startup.json");
	auto app = Se::CreateApplication();
	SE_PROFILE_END_SESSION();

	SE_PROFILE_BEGIN_SESSION("Runtime", "SaffronProfile-Runtime.json");
	app->Run();
	SE_PROFILE_END_SESSION();

	SE_PROFILE_BEGIN_SESSION("Shutdown", "SaffronProfile-Shutdown.json");
	app.reset();
	SE_PROFILE_END_SESSION();
}

#endif
