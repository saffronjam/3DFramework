#include "SaffronPCH.h"

#include "Saffron/Common/App.h"
#include "Saffron/Common/FileIOManager.h"
#include "Saffron/Common/Global.h"
#include "Saffron/Common/Run.h"

#include <bgfx/bgfx.h>

namespace Se
{
App* App::s_Instance = nullptr;


App::App(const Properties& properties)
{
	SE_ASSERT(!s_Instance, "App already exist");
	s_Instance = this;

	_preLoader = CreateShared<BatchLoader>("Preloader");

	_window = Window::Create(Window::Properties(properties.Name, properties.WindowWidth, properties.WindowHeight));

	_keyboard = CreateShared<Keyboard>();
	_mouse = CreateShared<Mouse>();


	SE_CORE_INFO("--- Saffron 2D Framework ---");
	SE_CORE_INFO("Creating application {0}", properties.Name);

	_window->Closed += SE_BIND_EVENT_FN(OnWindowClose);

	_renderer = CreateUnique<Renderer>(_window);
	_sceneRenderer = CreateUnique<SceneRenderer>();

	_gui = CreateUnique<Gui>();
	_gui->SetStyle(GuiStyle::Dark);

	FileIOManager::Init(_window);
	//Gui::Init(Filepath("../../../imgui.ini"));


	//_preLoader->Submit([]
	//				   {
	//					   Gui::SetStyle(Gui::Style::Dark);
	//				   }, "Initializing GUI");

	Global::Timer::Sync();


	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
}

App::~App()
{
	//Gui::Shutdown();
	SE_CORE_INFO("Shutting down");
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

void App::Run()
{
	OnInit();
	while (_running)
	{
		Frame();
	}
	_layerStack.Clear();

	OnShutdown();
}

void App::Exit()
{
	_preLoader->ForceExit();
	_running = false;
}

void App::OnGuiRender()
{
	/*if ( ImGui::Begin("Stats") )
	{
		const auto dt = Global::Clock::GetFrameTime();
		_fpsTimer += dt;
		if ( _fpsTimer.asSeconds() < 1.0f )
		{
			_storedFrameCount++;
			_storedFrametime += dt;
		}
		else
		{
			_cachedFps = static_cast<int>(static_cast<float>(_storedFrameCount) / _storedFrametime.asSeconds());
			_cachedSpf = sf::seconds(_storedFrametime.asSeconds() / static_cast<float>(_storedFrameCount));
			_storedFrameCount = 0;
			_storedFrametime = sf::Time::Zero;
			_fpsTimer = sf::Time::Zero;
		}

		Gui::BeginPropertyGrid();

		Gui::Property("Vendor", "SFML v.2.5.2");
		Gui::Property("Frametime", std::to_string(_cachedSpf.asMicroseconds() / 1000.0f) + " ms");
		Gui::Property("FPS", std::to_string(_cachedFps));

		Gui::EndPropertyGrid();
	}
	ImGui::End();*/
}

bool App::OnWindowClose(const WindowClosedEvent& event)
{
	Exit();
	return true;
}

void App::RunSplashScreen()
{
	_preLoader->Execute();

	/*SplashScreenPane splashScreenPane(_preLoader);
	while ( !splashScreenPane.IsFinished() )
	{
		_window.Clear();
		RenderTargetManager::ClearAll();
		Gui::Begin();
		splashScreenPane.OnUpdate();
		splashScreenPane.OnGuiRender();
		_window.HandleBufferedEvents();
		Gui::End();
		Run::Execute();
		RenderTargetManager::DisplayAll();
		_window.Display();
		Global::Clock::Restart();
		const auto step = Global::Clock::GetFrameTime().asSeconds();
		const auto duration = splashScreenPane.GetBatchLoader()->IsFinished() ? 0ll : std::max(0ll, static_cast<long long>(1000.0 / 60.0 - step));
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
	}*/
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
#elif defined(SE_PLATFORM_LINUX)
	return "Linux x64";
#endif
}

void App::Frame()
{
	if (!_preLoader->IsFinished())
	{
		RunSplashScreen();
		//_fadeIn.Start();
	}


	Global::Timer::Mark();
	_keyboard->OnUpdate();
	_mouse->OnUpdate();
	_window->HandleBufferedEvents();


	if (!_minimized)
	{
		//_gui->Begin();
		for (const auto& layer : _layerStack)
		{
			layer->OnUpdate();
		}
		for (const auto& layer : _layerStack)
		{
			layer->OnGuiRender();
		}

		//_fadeIn.OnUpdate();
		//_fadeIn.OnGuiRender();

		//_gui->End();


		_window->OnUpdate();
	}
	OnUpdate();
	_renderer->Execute();
	Run::Execute();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}
}
