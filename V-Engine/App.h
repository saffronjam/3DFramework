#pragma once

#include "Window.h"

class App
{
public:
	App();

	int Run();
private:
	void DoFrame();

private:
	ve::Window m_wnd;
};

