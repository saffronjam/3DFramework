#include "Application.h"

#include "Saffron/GL/GLCheck.h"

#include "Saffron/System/Log.h"

namespace Saffron
{

Application::Application()
	:
	m_pWnd(std::make_unique<Window>("Saffron Engine", 800, 600))
{
}

Application::~Application()
{
}

void Application::Run()
{
	while ( !m_pWnd->ShouldClose() )
	{
		m_pWnd->PollAllEvents();
		m_pWnd->BeginFrame();
		m_pWnd->EndFrame();
	}
}

}
