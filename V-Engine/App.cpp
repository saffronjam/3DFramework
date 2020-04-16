#include "App.h"

App::App()
	:
	m_wnd(800, 600, "V-Engine")
{
}

int App::Run()
{
	MSG msg;
	BOOL gResult;
	while ( (gResult = GetMessage(&msg, nullptr, 0, 0)) > 0 )
	{
		m_wnd.kbd.Update();
		m_wnd.mouse.Update();
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}

	if ( gResult == -1 )
		return -1;

	return (int)msg.wParam;
}

void App::DoFrame()
{
}
