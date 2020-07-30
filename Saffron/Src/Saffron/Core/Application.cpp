#include "Saffron/SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/System/Log.h"

namespace Se
{

Application *Application::m_sInstance = nullptr;

Application::Application()
	:
	m_pWindow(Se::Window::Create(Se::WindowProps())),
	m_Keyboard(*m_pWindow),
	m_Mouse(*m_pWindow),
	m_pImGuiLayer(CreateRef<ImGuiLayer>(m_pWindow))
{
	m_pWindow->AddEventHandler(this);

	SE_ASSERT(!m_sInstance, "Application already exist");
	m_sInstance = this;

	Renderer::Init();

	PushLayer(m_pImGuiLayer);
}

Application::~Application()
{
	Renderer::Shutdown();
}


void Application::Run()
{
	while ( m_Running )
	{
		dt = m_AppTimer.Mark();

		m_pWindow->HandleBufferedEvents();
		m_Keyboard.Update();
		m_Mouse.Update();

		if ( !m_Minimized )
		{
			auto mousePos = m_Mouse.GetPosition();
			mousePos.x /= m_pWindow->GetWidth();
			mousePos.y /= m_pWindow->GetHeight();

			RenderCommand::SetClearColor(glm::vec4(mousePos.x, mousePos.y, 0.0f, 1.0f));
			RenderCommand::Clear();

			// Normal updates and rendering
			for ( auto &layer : m_LayerStack )
				layer->OnUpdate(dt);

			// ImGui rendering
			m_pImGuiLayer->Begin();
			for ( auto &layer : m_LayerStack )
				layer->OnImGuiRender();
			m_pImGuiLayer->End();
		}

		m_pWindow->OnUpdate();
	}
}

void Application::Close()
{
}

void Application::PushLayer(const Ref<Layer> &layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(const Ref<Layer> &layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowCloseEvent>(SE_EVENT_FN(Application::OnWindowClose));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(Application::OnWindowResize));
}

const Ref<Window> &Application::GetWindow() const
{
	return m_pWindow;
}

const Ref<ImGuiLayer> &Application::GetImGuiLayer() const
{
	return m_pImGuiLayer;
}

void Application::OnWindowClose(const WindowCloseEvent &event)
{
	m_Running = false;
}

void Application::OnWindowResize(const WindowResizeEvent &event)
{
	if ( event.GetWidth() == 0 || event.GetHeight() == 0 )
	{
		m_Minimized = true;
	}
	m_Minimized = false;
}

}
