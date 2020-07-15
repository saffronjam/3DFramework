#pragma once

#include <random>

#include "Window.h"
#include "Clock.h"
#include "Drawable.h"
#include "GDIPlusMgr.h"
#include "Surface.h"
#include "GuiMgr.h"
#include "Camera.h"
#include "PointLight.h"
#include "Model.h"

class App
{
public:
	App();
	~App();

	int Run();
private:
	void DoFrame();

	void SpawnSimulationWindow() noexcept;

private:
	GuiMgr m_guiMgr;
	Window m_wnd;
	Clock m_clock;
	Camera m_camera;
	PointLight m_mainLight;

	float m_speedFactor;

	std::vector<std::unique_ptr<Drawable>> drawables;
	static constexpr int nDrawables = 180;

	GDIPlusMgr m_gdiPlusMgr;
};

