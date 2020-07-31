#include "Saffron/SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{

Application *Application::m_sInstance = nullptr;

Application::Application()
	:
	m_pWindow(Se::Window::Create(Se::WindowProps())),
	m_pImGuiLayer(CreateRef<ImGuiLayer>())
{
	m_pWindow->SetEventCallback(SE_EVENT_FN(Application::OnEvent));

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
	glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };

	while ( m_Running )
	{
		dt = m_AppTimer.Mark();

		m_Keyboard.OnUpdate();
		m_Mouse.OnUpdate();
		m_pWindow->HandleBufferedEvents();

		if ( !m_Minimized )
		{
			auto mousePos = m_Mouse.GetPosition();
			mousePos.x /= m_pWindow->GetWidth();
			mousePos.y /= m_pWindow->GetHeight();

			color.r = mousePos.x;
			color.g = mousePos.y;

			if ( m_Keyboard.IsPressed(SE_KEY_UP) )
				color.b += 0.1f;
			if ( m_Keyboard.IsPressed(SE_KEY_DOWN) )
				color.b -= 0.1f;

			RenderCommand::SetClearColor(color);
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
	if ( event.GetWidth() == 0 || event.GetHeight() == 0 )
	{
		m_Minimized = true;
	}
	m_Minimized = false;
}

}
