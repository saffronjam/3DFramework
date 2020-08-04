#include "Saffron/SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{

Application *Application::m_sInstance = nullptr;

Application::Application()
{
	SE_PROFILE_FUNCTION();

	m_pWindow = Window::Create(WindowProps());
	m_pImGuiLayer = CreateRef<ImGuiLayer>();


	m_pWindow->SetEventCallback(SE_EVENT_FN(Application::OnEvent));

	SE_ASSERT(!m_sInstance, "Application already exist");
	m_sInstance = this;

	Renderer::Init();

	PushLayer(m_pImGuiLayer);
}

Application::~Application()
{
	SE_PROFILE_FUNCTION();

	Renderer::Shutdown();
}


void Application::Run()
{
	SE_PROFILE_FUNCTION();

	glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };

	while ( m_Running )
	{
		SE_PROFILE_SCOPE("Run Loop");

		dt = m_AppTimer.Mark();

		{
			SE_PROFILE_SCOPE("Components OnUpdate");

			m_Keyboard.OnUpdate();
			m_Mouse.OnUpdate();
			m_pWindow->HandleBufferedEvents();
		}

		if ( !m_Minimized )
		{
			// Normal updates and rendering
			{
				SE_PROFILE_SCOPE("LayerStack OnUpdate");

				for ( auto &layer : m_LayerStack )
					layer->OnUpdate(dt);
			}

			// ImGui rendering
			{
				SE_PROFILE_SCOPE("LayerStack OnImGuiRender");

				m_pImGuiLayer->Begin();
				for ( auto &layer : m_LayerStack )
					layer->OnImGuiRender();
				m_pImGuiLayer->End();
			}
		}
		m_pWindow->OnUpdate();
	}
}

void Application::Close()
{
}

void Application::PushLayer(const Ref<Layer> &layer)
{
	SE_PROFILE_FUNCTION();

	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(const Ref<Layer> &layer)
{
	SE_PROFILE_FUNCTION();

	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::OnEvent(const Event &event)
{
	SE_PROFILE_FUNCTION();

	// TODO: Optimize this if event is handled?

	// Application events
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowCloseEvent>(SE_EVENT_FN(Application::OnWindowClose));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(Application::OnWindowResize));

	// Window events
	m_pWindow->OnEvent(event);

	// Keyboard events
	m_Keyboard.OnEvent(event);

	// Mouse events
	m_Mouse.OnEvent(event);

	// Renderer events
	Renderer::OnEvent(event);

	// Layer events
	for ( const auto &layer : m_LayerStack )
	{
		layer->OnEvent(event);
	}
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
	SE_PROFILE_FUNCTION();

	if ( event.GetWidth() == 0 || event.GetHeight() == 0 )
	{
		m_Minimized = true;
	}
	m_Minimized = false;
}

}
