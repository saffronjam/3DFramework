#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Core/LayerStack.h"
#include "Saffron/Core/Timer.h"
#include "Saffron/Core/Window.h"
#include "Saffron/Gui/GuiLayer.h"

namespace Se {



class Application
{
public:
	struct Properties
	{
		std::string Name;
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

	std::filesystem::path OpenFile(const char *filter = "All\0*.*\0") const;
	std::filesystem::path SaveFile(const char *filter = "All\0*.*\0") const;

	Window &GetWindow() { return *m_Window; }
	static Application &Get() { return *s_Instance; }
	float GetTime() const; // TODO: This should be in "Platform"

	static const char *GetConfigurationName();
	static const char *GetPlatformName();

private:
	bool OnWindowClose(const WindowCloseEvent &event);

private:
	Ref<Window> m_Window;
	bool m_Running = true, m_Minimized = false;
	LayerStack m_LayerStack;
	GuiLayer *m_GuiLayer;

	Time ts;

	static Application *s_Instance;
};

// Implemented by CLIENT
Application *CreateApplication();

}