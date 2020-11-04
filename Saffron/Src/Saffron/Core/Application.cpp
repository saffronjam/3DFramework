#include "SaffronPCH.h"


#include "Saffron/Core/Application.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Gui/SplashScreen.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Script/ScriptEngine.h"

namespace Se {

Application *Application::s_Instance = nullptr;

Application::Application(const Properties &properties)
{
	SE_ASSERT(!s_Instance, "Application already exist");
	s_Instance = this;

	m_Window = Window::Create(Window::Properties(properties.Name, properties.WindowWidth, properties.WindowHeight));
	m_Window->SetEventCallback(SE_BIND_EVENT_FN(OnEvent));
	m_Window->SetVSync(true);
	m_Window->SetWindowIcon("Assets/Editor/Saffron_windowIcon.png");
	m_Window->HandleBufferedEvents();

	m_GuiLayer = new GuiLayer("Gui");
	PushOverlay(m_GuiLayer);

	Renderer::Init();
	Renderer::WaitAndRender();
	ScriptEngine::Init("Assets/Scripts/ExampleApp.dll");
	FileIOManager::Init(*m_Window);

	ts = GlobalTimer::Mark();
}

Application::~Application()
{
	ScriptEngine::Shutdown();
}

void Application::PushLayer(Layer *layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer *layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::RenderGui()
{
	m_GuiLayer->Begin();

	for ( Layer *layer : m_LayerStack )
		layer->OnGuiRender();

	m_GuiLayer->End();
}

void Application::Run()
{
	OnInit();
	BatchLoader::GetPreloader()->Submit([] {}, "Finalizing");
	BatchLoader::GetPreloader()->SetOnFinishCallback([&]
													 {
														 m_ViewingSplashScreen = false;
														 ScriptEngine::DetachThread();
													 });

	Thread preloaderWorker([&] {
		ScriptEngine::AttachThread();
		BatchLoader::GetPreloader()->Execute();
						   });
	SplashScreen splashScreen;
	while ( m_ViewingSplashScreen ? m_ViewingSplashScreen : !splashScreen.IsIdle() )
	{
		m_GuiLayer->Begin();
		splashScreen.OnGuiRender();
		m_Window->OnUpdate();
		m_Window->HandleBufferedEvents();
		m_GuiLayer->End();
		Renderer::WaitAndRender();
		GlobalTimer::Mark();
		const auto duration = m_ViewingSplashScreen ? std::max(0ll, static_cast<long long>(1000.0 / 60.0 - GlobalTimer::GetStep().ms())) : 0ll;
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		GlobalTimer::Sync();
	}
	preloaderWorker.join();
	BatchLoader::InvalidatePreloader();

	while ( m_Running )
	{
		m_Window->HandleBufferedEvents();
		if ( !m_Minimized )
		{
			for ( Layer *layer : m_LayerStack )
				layer->OnUpdate();

			ScriptEngine::OnUpdate();
			Input::OnUpdate();
			m_Window->OnUpdate();

			// Render ImGui on render thread
			Application *app = this;
			Renderer::Submit([app]() { app->RenderGui(); });

			Renderer::WaitAndRender();
		}
		GlobalTimer::Mark();
	};

	OnShutdown();
}

void Application::Exit()
{
	m_Running = false;
}

void Application::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowCloseEvent>(SE_BIND_EVENT_FN(OnWindowClose));

	Input::OnEvent(event);
	m_Window->OnEvent(event);

	for ( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
	{
		if ( event.Handled )
			break;
		(*--it)->OnEvent(event);
	}
}

bool Application::OnWindowClose(const WindowCloseEvent &event)
{
	m_Running = false;
	return true;
}

const char *Application::GetConfigurationName()
{
#if defined(SE_DEBUG)
	return "Debug";
#elif defined(SE_RELEASE)
	return "Release";
#elif defined(SE_DIST)
	return "Dist";
#else
#error Undefined configuration?
#endif
}

const char *Application::GetPlatformName()
{
#if defined(SE_PLATFORM_WINDOWS)
	return "Windows x64";
#else
#error Undefined platform?
#endif
}

}