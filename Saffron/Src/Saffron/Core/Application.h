#pragma once

#include "Saffron/SaffronPCH.h"
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
public:
	Application();
	virtual ~Application();

	void Run();
	void Close();

	void PushLayer(const Ref<Layer> &layer);
	void PushOverlay(const Ref<Layer> &layer);

	virtual void OnEvent(const Event &event);

	const Ref<Window> &GetWindow() const;
	const Ref<ImGuiLayer> &GetImGuiLayer() const;

	static Application &Get() { return *m_sInstance; }

private:
	void OnWindowClose(const WindowCloseEvent &event);
	void OnWindowResize(const WindowResizeEvent &event);

protected:
	Ref<Window> m_pWindow;
	Keyboard m_Keyboard;
	Mouse m_Mouse;
	Time dt;

private:
	bool m_Running = true;
	bool m_Minimized = false;
	Timer m_AppTimer;

	Ref<ImGuiLayer> m_pImGuiLayer;
	LayerStack m_LayerStack;

	static Application *m_sInstance;
};

Ref<Application> CreateApplication();

}
