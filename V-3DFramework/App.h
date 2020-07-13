#pragma once

#include <random>

#include "Window.h"
#include "Clock.h"
#include "Drawable.h"
#include "GDIPlusMgr.h"
#include "Surface.h"

class App
{
public:
	App();
	~App();

	int Run();
private:
	void DoFrame();

private:
	Window m_wnd;
	Clock m_clock;

	std::vector<std::unique_ptr<Drawable>> drawables;
	static constexpr int nDrawables = 180;

	GDIPlusMgr m_gdiPlusMgr;
};

