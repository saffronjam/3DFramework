#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Core/LayerStack.h"
#include "Saffron/Core/Window.h"
#include "Saffron/Gui/GuiLayer.h"

namespace Se
{
class Application
{
public:
	struct Properties
	{
		String Name;
		Uint32 WindowWidth, WindowHeight;
	};

public:
	Application(const Properties &properties = { "Saffron Engine", 1280, 720 });
	virtual ~Application();

	void Run();
	void Exit();

	virtual void OnInit() {}
	virtual void OnShutdown() {}
	virtual void OnUpdate(Time ts) {}
	virtual void OnEvent(const Event &event);

	void PushLayer(Layer *layer);
	void PushOverlay(Layer *layer);
	void RenderGui();

	Window &GetWindow() { return *m_Window; }
	static Application &Get() { return *s_Instance; }

	static const char *GetConfigurationName();
	static const char *GetPlatformName();

private:
	bool OnWindowClose(const WindowCloseEvent &event);

private:
	Shared<Window> m_Window;
	bool m_Running = true, m_Minimized = false;
	LayerStack m_LayerStack;
	GuiLayer *m_GuiLayer;

	Time ts;

	static Application *s_Instance;
};

// Implemented by client
Application *CreateApplication();

}