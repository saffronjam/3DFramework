#include "SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Core/Run.h"
#include "Saffron/Core/ScopedLock.h"
#include "Saffron/Editor/SplashScreenPane.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/Serialize/ApplicationSerializer.h"
#include "Saffron/Scene/EditorScene.h"
namespace Se {

Application *Application::s_Instance = nullptr;


Application::Application(const Properties &properties)
	: m_PreLoader(CreateShared<BatchLoader>("Preloader"))
{
	SE_ASSERT(!s_Instance, "Application already exist");
	s_Instance = this;

	m_Window = Window::Create(Window::Properties(properties.Name, properties.WindowWidth, properties.WindowHeight));
	m_Window->GetSignal(Window::Signals::OnEvent).Connect(SE_BIND_EVENT_FN(OnEvent));
	m_Window->SetVSync(true);
	m_Window->SetWindowIcon("Resources/Assets/Editor/Saffron_windowIcon.png");
	m_Window->HandleBufferedEvents();

	Renderer::Init();
	Renderer::Execute();
	ScriptEngine::Init();
	FileIOManager::Init(*m_Window);
	Gui::Init();

	m_PreLoader->GetSignal(BatchLoader::Signals::OnStart).Connect([] {ScriptEngine::AttachThread(); });
	m_PreLoader->GetSignal(BatchLoader::Signals::OnFinish).Connect([] {ScriptEngine::DetachThread();  });

	m_PreLoader->Submit([this]
						{
							ApplicationSerializer serializer(*this);
							serializer.Deserialize("Application/ApplicationProperties.sap");
						}, "Deserializing Engine Properties");

	m_PreLoader->Submit([]
						{
							Gui::SetStyle(Gui::Style::Dark);
						}, "Initializing GUI");

	ts = GlobalTimer::Mark();
}

Application::~Application()
{
	ScriptEngine::Shutdown();
	Gui::Shutdown();
	const ApplicationSerializer serializer(*this);
	serializer.Serialize("Application/ApplicationProperties.sap");
	ResourceManager::Clear();
	FramebufferPool::GetGlobal()->Clear();
}

void Application::PushLayer(std::shared_ptr<Layer> layer)
{
	m_LayerStack.PushLayer(layer, m_PreLoader);
}

void Application::PushOverlay(std::shared_ptr<Layer> overlay)
{
	m_LayerStack.PushOverlay(overlay, m_PreLoader);
}

void Application::PopLayer(int count)
{
	m_LayerStack.PopLayer(count);
}

void Application::PopOverlay(int count)
{
	m_LayerStack.PopOverlay(count);
}

void Application::EraseLayer(std::shared_ptr<Layer> layer)
{
	m_LayerStack.EraseLayer(layer);
}

void Application::EraseOverlay(std::shared_ptr<Layer> overlay)
{
	m_LayerStack.EraseOverlay(overlay);
}

void Application::RenderGui()
{
	Gui::Begin();

	for ( const auto &layer : m_LayerStack )
	{
		layer->OnGuiRender();
	}

	Gui::End();
}

void Application::Run()
{
	OnInit();

	while ( m_Running )
	{
		if ( !m_PreLoader->IsFinished() )
		{
			RunSplashScreen();
		}

		m_Window->HandleBufferedEvents();
		if ( !m_Minimized )
		{
			for ( const auto &layer : m_LayerStack )
			{
				layer->OnUpdate();
			}
			ScriptEngine::OnUpdate();
			Input::OnUpdate();
			m_Window->OnUpdate();
			Renderer::Submit([this]() { RenderGui(); });
			Renderer::Execute();
		}
		OnUpdate();
		Run::Execute();
		GlobalTimer::Mark();
	}

	m_LayerStack.Clear();

	OnShutdown();
}

void Application::Exit()
{
	m_PreLoader->ForceExit();
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
	Exit();
	return true;
}

void Application::AddProject(const std::shared_ptr<Project> &project)
{
	if ( std::find(m_RecentProjectList.begin(), m_RecentProjectList.end(), project) == m_RecentProjectList.end() )
	{
		m_RecentProjectList.push_back(project);
	}
}

void Application::RemoveProject(const std::shared_ptr<Project> &project)
{
	m_RecentProjectList.erase(std::remove(m_RecentProjectList.begin(), m_RecentProjectList.end(), project), m_RecentProjectList.end());
}

const ArrayList<std::shared_ptr<Project> > &Application::GetRecentProjectList() const
{
	std::sort(m_RecentProjectList.begin(), m_RecentProjectList.end(), [](const auto &first, const auto &second)
			  {
				  return first->LastOpened() > second->LastOpened();
			  });
	return m_RecentProjectList;
}

const std::shared_ptr<Project> &Application::GetActiveProject() const
{
	SE_CORE_ASSERT(m_ActiveProject, "Tried to fetch active project when there was none");
	return m_ActiveProject;
}

void Application::SetActiveProject(const std::shared_ptr<Project> &project)
{
	const auto iter = std::find(m_RecentProjectList.begin(), m_RecentProjectList.end(), project);
	SE_CORE_ASSERT(iter != m_RecentProjectList.end(), "Tried loading an invalid project");
	m_ActiveProject = *iter;
	m_ActiveProject->SyncLastOpened();
}

String Application::GetConfigurationName()
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

String Application::GetPlatformName()
{
#if defined(SE_PLATFORM_WINDOWS)
	return "Windows x64";
#else
#error Undefined platform?
#endif
}

void Application::RunSplashScreen()
{
	m_PreLoader->Execute();

	SplashScreenPane splashScreenPane(m_PreLoader);
	while ( !splashScreenPane.IsFinished() )
	{
		Gui::Begin();
		splashScreenPane.OnGuiRender();
		m_Window->OnUpdate();
		m_Window->HandleBufferedEvents();
		Gui::End();
		Renderer::Execute();
		Run::Execute();
		GlobalTimer::Mark();
		const auto duration = splashScreenPane.GetBatchLoader()->IsFinished() ? 0ll : std::max(0ll, static_cast<long long>(1000.0 / 60.0 - GlobalTimer::GetStep().ms()));
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		GlobalTimer::Sync();
	}
}
}
