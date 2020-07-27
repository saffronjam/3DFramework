#include "Application.h"

#include "Saffron/System/Log.h"

namespace Saffron
{

Application *Application::m_sInstance = nullptr;

Application::Application()
	:
	m_pWnd(std::make_unique<Window>("Saffron Engine", 800, 600)),
	m_pImGuiLayer(Layer::Create<ImGuiLayer>())
{
	SE_ASSERT(!m_sInstance, "Application already exist");
	m_sInstance = this;
}

Application::~Application()
{
}

void Application::Run()
{
	while ( m_Running )
	{
		auto dt = m_AppTimer.Mark();

		m_pWnd->HandleBufferedEvents();

		if ( !m_Minimized )
		{
			{
				for ( auto &layer : m_LayerStack )
					layer->OnUpdate(dt);
			}

			m_pImGuiLayer->Begin();
			{

				for ( auto &layer : m_LayerStack )
					layer->OnImGuiRender();
			}
			m_pImGuiLayer->End();
		}

		m_pWnd->OnUpdate();
	}
}

void Application::Close()
{
}

void Application::OnEvent(const Event::Ptr pEvent)
{
}

Window::Ptr Application::GetWindow() const
{
	return m_pWnd;
}

ImGuiLayer::Ptr Application::GetImGuiLayer() const
{
}

void Application::OnWindowClose(const WindowCloseEvent::Ptr pEvent)
{
	m_Running = false;
}

void Application::OnWindowResize(const WindowResizeEvent::Ptr pEvent)
{
	if ( pEvent->GetWidth() == 0 || pEvent->GetHeight() == 0 )
	{
		m_Minimized = true;
	}
	m_Minimized = false;
}

}
