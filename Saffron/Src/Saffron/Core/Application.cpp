#include "SaffronPCH.h"


#include "Saffron/Core/Application.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Core/Run.h"
#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Editor/SplashScreenPane.h"
#include "Saffron/Engine/EngineSerializer.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Script/ScriptEngine.h"

namespace Se {

Application *Application::s_Instance = nullptr;

Application::Application(const Properties &properties)
	: m_PreLoader(Shared<BatchLoader>::Create("Preloader"))
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
	Renderer::Execute();
	ScriptEngine::Init("Assets/Scripts/ExampleApp.dll");
	FileIOManager::Init(*m_Window);
	EngineSerializer::Deserialize("Engine/EngineProperties.sen");

	ts = GlobalTimer::Mark();
}

Application::~Application()
{
	ScriptEngine::Shutdown();
	EngineSerializer::Serialize("Engine/EngineProperties.sen");
}

void Application::PushLayer(Layer *layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach(m_PreLoader);
}

void Application::PushOverlay(Layer *layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach(m_PreLoader);
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

	m_PreLoader->SetOnStartCallback([] {ScriptEngine::AttachThread(); });
	m_PreLoader->SetOnFinishCallback([] {ScriptEngine::DetachThread(); });

	m_PreLoader->Execute();

	SplashScreenPane splashScreenPane(m_PreLoader);
	while ( !splashScreenPane.IsFinished() )
	{
		m_GuiLayer->Begin();
		splashScreenPane.OnGuiRender();
		m_Window->OnUpdate();
		m_Window->HandleBufferedEvents();
		m_GuiLayer->End();
		Renderer::Execute();
		Run::Execute();
		GlobalTimer::Mark();
		const auto duration = splashScreenPane.GetBatchLoader()->IsFinished() ? 0ll : std::max(0ll, static_cast<long long>(1000.0 / 60.0 - GlobalTimer::GetStep().ms()));
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		GlobalTimer::Sync();

	}

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

			Renderer::Execute();
		}
		Run::Execute();
		GlobalTimer::Mark();
	}

	for ( auto &layer : m_LayerStack )
	{
		layer->OnDetach();
	}

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