#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Graphics/Window.h"

namespace Saffron
{
class SAFFRON_API Application
{
public:
	using Ptr = std::shared_ptr<Application>;
public:
	Application();
	virtual ~Application();

	void Run();

private:
	std::unique_ptr<Window> m_pWnd;


};

Application::Ptr CreateApplication();

}

