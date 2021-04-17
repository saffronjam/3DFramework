#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Project.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/LayerStack.h"
#include "Saffron/Core/Window.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Gui/Gui.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

#include "Saffron/Script/ScriptEngine.h"

namespace Se
{
class App : public SingleTon<App>
{
	friend class AppSerializer;

public:
	struct Properties
	{
		String Name;
		Uint32 WindowWidth, WindowHeight;
	};

public:
	App(const Properties& properties = {"Saffron Engine", 1280, 720});
	virtual ~App();

	void Run();
	void Exit();

	virtual void OnInit()
	{
	}

	virtual void OnShutdown()
	{
	}

	virtual void OnUpdate()
	{
	}

	void PushLayer(Shared<Layer> layer);
	void PushOverlay(Shared<Layer> overlay);
	void PopLayer(int count = 1);
	void PopOverlay(int count = 1);
	void EraseLayer(Shared<Layer> layer);
	void EraseOverlay(Shared<Layer> overlay);

	void RenderGui();

	Window& GetWindow();
	const Window& GetWindow() const;

	void AddProject(const Shared<Project>& project);
	void RemoveProject(const Shared<Project>& project);

	const ArrayList<Shared<Project>>& GetRecentProjectList() const;
	const Shared<Project>& GetActiveProject() const;
	void SetActiveProject(const Shared<Project>& project);

	static String GetConfigurationName();
	static String GetPlatformName();

private:
	void RunSplashScreen();

	bool OnWindowClose(const WindowClosedEvent& event);

protected:
	Shared<BatchLoader> _preLoader;

private:
	Unique<Renderer> _renderer;
	Unique<SceneRenderer> _sceneRenderer;
	Unique<ScriptEngine> _scriptEngine;

	Shared<Window> _window;
	Unique<Gui> _gui;
	Unique<FileIOManager> _fileIOManager;

	bool _running = true, _minimized = false;
	LayerStack _layerStack;
	Mutex _finalPreloaderMessageMutex;

	Unique<Mouse> _mouse;
	Unique<Keyboard> _keyboard;

	mutable ArrayList<Shared<Project>> _recentProjectList;
	Shared<Project> _activeProject = nullptr;
};

// Implemented by client
App* CreateApplication();
}
