#include "SaffronPCH.h"

#include "Saffron/Core/App.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Run.h"
#include "Saffron/Core/ScopedLock.h"

#include "Saffron/Editor/SplashScreenPane.h"

#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"

#include "Saffron/Rendering/Renderer.h"

#include "Saffron/Resource/ResourceManager.h"

#include "Saffron/Serialize/AppSerializer.h"
#include "Saffron/Scene/EditorScene.h"

namespace Se
{
App::App(const Properties& properties) :
	SingleTon(this),
	_preLoader(Shared<BatchLoader>::Create("Preloader"))
{
	_window = Window::Create(Window::Properties(properties.Name, properties.WindowWidth, properties.WindowHeight));

	_window->Closed += SE_BIND_EVENT_FN(App::OnWindowClose);
	_window->SetVSync(false);
	_window->Maximize();
	_window->SetWindowIcon("Editor/Saffron_windowIcon.png");
	_window->HandleBufferedEvents();

	Run::Execute();

	_renderer = Shared<Renderer>::Create();
	_sceneRenderer = CreateUnique<SceneRenderer>();

	_scriptEngine = CreateUnique<ScriptEngine>();
	_fileIOManager = CreateUnique<FileIOManager>(_window);

	_keyboard = CreateUnique<Keyboard>();
	_mouse = CreateUnique<Mouse>();

	_gui = CreateUnique<Gui>();

	_preLoader->Started += []
	{
		ScriptEngine::Instance().AttachThread();
		return false;
	};
	_preLoader->Finished += []
	{
		ScriptEngine::Instance().DetachThread();
		return false;
	};

	_preLoader->Submit([this]
	{
		AppSerializer serializer(*this);
		serializer.Deserialize("App/ApplicationProperties.sap");
	}, "Deserializing Engine Properties");

	_preLoader->Submit([]
	{
		Gui::SetStyle(Gui::Style::Dark);
	}, "Initializing GUI");

	Renderer::WaitAndRender();
}

App::~App()
{
	//const AppSerializer serializer(*this);
	//serializer.Serialize("App/ApplicationProperties.sap");
	ResourceManager::Clear();
}

void App::PushLayer(Shared<Layer> layer)
{
	_layerStack.PushLayer(layer, _preLoader);
}

void App::PushOverlay(Shared<Layer> overlay)
{
	_layerStack.PushOverlay(overlay, _preLoader);
}

void App::PopLayer(int count)
{
	_layerStack.PopLayer(count);
}

void App::PopOverlay(int count)
{
	_layerStack.PopOverlay(count);
}

void App::EraseLayer(Shared<Layer> layer)
{
	_layerStack.EraseLayer(layer);
}

void App::EraseOverlay(Shared<Layer> overlay)
{
	_layerStack.EraseOverlay(overlay);
}

Window& App::GetWindow()
{
	return *_window;
}

const Window& App::GetWindow() const
{
	return *_window;
}

Shared<Renderer> App::GetRenderer() const
{
	return _renderer;
}

void App::Run()
{
	OnInit();

	while (_running)
	{
		if (!_preLoader->IsFinished())
		{
			RunSplashScreen();
		}

		Global::Timer::Mark();

		_gui->Begin();
		if (!_minimized)
		{
			for (auto& layer : _layerStack)
			{
				layer->OnUpdate();
			}
			Renderer::Submit([this]()
			{
				for (auto& layer : _layerStack)
				{
					layer->OnGuiRender();
				}
			});

			Renderer::WaitAndRender();
		}

		ScriptEngine::Instance().OnUpdate();

		_keyboard->OnUpdate();
		_mouse->OnUpdate();
		OnUpdate();
		_gui->End();

		_window->OnUpdate();
		Run::Execute();

		_window->HandleBufferedEvents();
	}

	_layerStack.Clear();

	OnShutdown();
}

void App::Exit()
{
	_preLoader->ForceExit();
	_running = false;
}

bool App::OnWindowClose(const WindowClosedEvent& event)
{
	Exit();
	return true;
}

void App::AddProject(const Shared<Project>& project)
{
	if (std::find(_recentProjectList.begin(), _recentProjectList.end(), project) == _recentProjectList.end())
	{
		_recentProjectList.push_back(project);
	}
}

void App::RemoveProject(const Shared<Project>& project)
{
	_recentProjectList.erase(std::remove(_recentProjectList.begin(), _recentProjectList.end(), project),
	                         _recentProjectList.end());
}

const ArrayList<Shared<Project>>& App::GetRecentProjectList() const
{
	std::sort(_recentProjectList.begin(), _recentProjectList.end(), [](const auto& first, const auto& second)
	{
		return first->LastOpened() > second->LastOpened();
	});
	return _recentProjectList;
}

const Shared<Project>& App::GetActiveProject() const
{
	Debug::Assert(_activeProject, "Tried to fetch active project when there was none");
	return _activeProject;
}

void App::SetActiveProject(const Shared<Project>& project)
{
	const auto iter = std::find(_recentProjectList.begin(), _recentProjectList.end(), project);
	Debug::Assert(iter != _recentProjectList.end(), "Tried loading an invalid project");
	_activeProject = *iter;
	_activeProject->SyncLastOpened();
}

String App::GetConfigurationName()
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

String App::GetPlatformName()
{
#if defined(SE_PLATFORM_WINDOWS)
	return "Windows x64";
#else
#error Undefined platform?
#endif
}

void App::RunSplashScreen()
{
	_preLoader->Execute();

	while (!_preLoader->IsFinished())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	/*SplashScreenPane splashScreenPane(_preLoader);
	while (!splashScreenPane.IsFinished())
	{
		_gui->Begin();
		splashScreenPane.OnGuiRender();
		_window->OnUpdate();
		_window->HandleBufferedEvents();
		_gui->End();
		Renderer::WaitAndRender();
		Run::Execute();
		Global::Timer::Mark();
		const auto duration = splashScreenPane.GetBatchLoader()->IsFinished()
			                      ? 0ll
			                      : std::max(
				                      0ll, static_cast<long long>(1000.0 / 60.0 - Global::Timer::GetStep().ms()));
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		Global::Timer::Sync();
	}*/
}
}
