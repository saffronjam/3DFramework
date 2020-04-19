#pragma once

#include <random>

#include "Window.h"
#include "Clock.h"
#include "Drawable.h"

class App
{
public:
	App();
	~App();

	int Run();
private:
	void DoFrame();

private:
	ve::Window m_wnd;
	ve::Clock m_clock;

	std::vector<std::unique_ptr<ve::Drawable>> drawables;
	static constexpr int nDrawables = 180;
};

