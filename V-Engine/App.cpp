#include "App.h"

App::App()
	:
	m_wnd(800, 600, "V-Engine")
{
}

int App::Run()
{
	while ( true )
	{
		m_wnd.kbd.Update();
		m_wnd.mouse.Update();
		if ( const auto exitCode = ve::Window::ProcessMessages() )
			return *exitCode;
		DoFrame();

	}
}

void App::DoFrame()
{
	const float c = sin(m_clock.PeekDeltatime().asSeconds()) / 2.0f + 0.5;
	m_wnd.gfx.ClearBuffer(c, c, 1.0f);
	m_wnd.gfx.EndFrame();
}
