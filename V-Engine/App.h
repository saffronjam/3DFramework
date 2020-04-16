#pragma once

#include <iomanip>

#include "Window.h"
#include "Clock.h"

class App
{
public:
	App();

	int Run();
private:
	void DoFrame();

private:
	ve::Window m_wnd;
	ve::Clock m_clock;
	ve::Time m_dt;
};

