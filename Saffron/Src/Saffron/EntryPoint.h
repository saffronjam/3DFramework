#pragma once

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Engine.h"

#ifdef SE_PLATFORM_WINDOWS

extern Se::Application::Ptr Se::CreateApplication();

int main()
{
	Se::Engine::Initialize(Se::Engine::Subsystem::Graphics);
	Se::Log::Init();

	auto app = Se::CreateApplication();
	app->Run();
}

#endif