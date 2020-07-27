#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/Graphics/Window.h"
#include "Saffron/Gui/ImGuiLayer.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/Graphics/LayerStack.h"
#include "Saffron/System/Timer.h"

namespace Se
{
class SAFFRON_API Application : public EventHandler
{
public:
	using Ptr = std::shared_ptr<Application>;
public:
	Application();
	virtual ~Application() = default;

	void Run();
	void Close();

	void PushLayer(const Layer::Ptr &layer);
	void PushOverlay(const Layer::Ptr &layer);

	void OnEvent(const Event::Ptr &pEvent) override;

	const Window::Ptr &GetWindow() const;
	ImGuiLayer::Ptr GetImGuiLayer() const;

	static Application &Get() { return *m_sInstance; }

private:
	void OnWindowClose(const WindowCloseEvent::Ptr &pEvent);
	void OnWindowResize(const WindowResizeEvent::Ptr &pEvent);

private:
	bool m_Running = true;
	bool m_Minimized = false;
	Timer m_AppTimer;

	Window::Ptr m_pWindow;
	ImGuiLayer::Ptr m_pImGuiLayer;
	LayerStack m_LayerStack;

	static Application *m_sInstance;
};

Application::Ptr CreateApplication();

}
