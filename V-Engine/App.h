#pragma once

#include "Window.h"
#include "Clock.h"
#include "Box.h"

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

	std::vector<std::unique_ptr<ve::Box>> boxes;
};

