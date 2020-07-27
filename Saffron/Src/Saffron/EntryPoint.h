#pragma once

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Engine.h"

#ifdef SE_PLATFORM_WINDOWS

extern Saffron::Application::Ptr Saffron::CreateApplication();

int main()
{
	Saffron::Engine::Initialize(Saffron::Engine::Subsystem::Graphics);
	Saffron::Log::Init();

	auto app = Saffron::CreateApplication();
	app->Run();
}

#endif