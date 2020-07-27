#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Graphics/Window.h"
#include "Saffron/Gui/ImGuiLayer.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/Graphics/LayerStack.h"
#include "Saffron/System/Timer.h"

namespace Saffron
{
class SAFFRON_API Application : public EventHandler
{
public:
	using Ptr = std::shared_ptr<Application>;
public:
	Application();
	virtual ~Application();

	void Run();
	void Close();

	void OnEvent(const Event::Ptr pEvent) override;

	Window::Ptr GetWindow() const;
	ImGuiLayer::Ptr GetImGuiLayer() const;

	static Application &Get() { return *m_sInstance; }

private:
	void OnWindowClose(const WindowCloseEvent::Ptr pEvent);
	void OnWindowResize(const WindowResizeEvent::Ptr pEvent);

private:
	bool m_Running = true;
	bool m_Minimized = false;
	Timer m_AppTimer;

	Window::Ptr m_pWnd;
	ImGuiLayer::Ptr m_pImGuiLayer;
	LayerStack m_LayerStack;

	static Application *m_sInstance;
};

Application::Ptr CreateApplication();

}
