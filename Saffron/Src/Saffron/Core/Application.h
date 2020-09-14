#pragma once

#include "Saffron/Config.h"
#include "Saffron/Core/Event/WindowEvent.h"
#include "Saffron/Graphics/Window.h"
#include "Saffron/Gui/ImGuiLayer.h"
#include "Saffron/Graphics/LayerStack.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/System/Timer.h"

namespace Se
{
class Application
{
	struct Properties
	{
		std::string Name;
		Uint32 WindowWidth, WindowHeight;
	};

public:
	explicit Application(const Properties &properties = { "Saffron Engine", 1280, 720 });
	virtual ~Application();

	void Run();
	void Close();

	void PushLayer(Layer *layer);
	void PushOverlay(Layer *layer);
	void RenderImGui();

	virtual void OnInit() {}
	virtual void OnShutdown() {}
	virtual void OnUpdate(Time ts) {}
	virtual void OnEvent(const Event &event);

	const Ref<Window> &GetWindow() const;
	static Application &Get() { return *m_sInstance; }

	static const char *GetConfigurationName();
	static const char *GetPlatformName();

	std::string OpenFile(const char *filter = "All\0*.*\0") const;
	std::string SaveFile(const char *filter = "All\0*.*\0") const;

private:
	void OnWindowClose(const WindowCloseEvent &event);
	void OnWindowResize(const WindowResizeEvent &event);

protected:
	Ref<Window> m_Window;
	Keyboard m_Keyboard;
	Mouse m_Mouse;
	Time dt;

private:
	bool m_Running = true;
	bool m_Minimized = false;
	Timer m_AppTimer;

	// TODO: Make this a ref instead of raw pointer
	ImGuiLayer *m_ImGuiLayer;
	LayerStack m_LayerStack;

	static Application *m_sInstance;
};

Ref<Application> CreateApplication();

}
