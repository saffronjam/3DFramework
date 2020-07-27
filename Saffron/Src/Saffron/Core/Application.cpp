#include "Saffron/SaffronPCH.h"
#include "Saffron/Core/Application.h"
#include "Saffron/System/Log.h"

namespace Se
{

Application *Application::m_sInstance = nullptr;

Application::Application()
	:
	m_pWindow(CreateRef<Window>("Saffron Engine", 800, 600)),
	m_pImGuiLayer(CreateRef<ImGuiLayer>(m_pWindow))
{
	m_pWindow->AddEventHandler(this);

	SE_ASSERT(!m_sInstance, "Application already exist");
	m_sInstance = this;

	PushLayer(m_pImGuiLayer);
}


void Application::Run()
{
	while ( m_Running )
	{
		const auto dt = m_AppTimer.Mark();

		m_pWindow->HandleBufferedEvents();
		m_pWindow->kbd.Update();
		m_pWindow->mouse.Update();

		if ( !m_Minimized )
		{
			m_pImGuiLayer->Begin();
			{
				for ( auto &layer : m_LayerStack )
					layer->OnUpdate(dt);
			}
			{
				for ( auto &layer : m_LayerStack )
					layer->OnImGuiRender();
			}
			m_pImGuiLayer->End();
		}

		m_pWindow->OnUpdate();
	}
}

void Application::Close()
{
}

void Application::PushLayer(const Layer::Ptr &layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(const Layer::Ptr &layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::OnEvent(const Event::Ptr &pEvent)
{
	const EventDispatcher dispatcher(pEvent);
	dispatcher.Try<WindowCloseEvent>(SE_EVENT_FN(Application::OnWindowClose));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(Application::OnWindowResize));
}

const Window::Ptr &Application::GetWindow() const
{
	return m_pWindow;
}

ImGuiLayer::Ptr Application::GetImGuiLayer() const
{
	return m_pImGuiLayer;
}

void Application::OnWindowClose(const WindowCloseEvent::Ptr &pEvent)
{
	m_Running = false;
}

void Application::OnWindowResize(const WindowResizeEvent::Ptr &pEvent)
{
	if ( pEvent->GetWidth() == 0 || pEvent->GetHeight() == 0 )
	{
		m_Minimized = true;
	}
	m_Minimized = false;
}

}
