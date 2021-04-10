#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Common/LayerStack.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
class Scene;

class App
{
	friend class ApplicationSerializer;

public:
	struct Properties
	{
		String Name;
		Uint32 WindowWidth, WindowHeight;
	};

public:
	explicit App(const Properties& properties = {"Saffron 3D", 1280, 720});
	virtual ~App();

	void Run();
	void Exit();

	virtual void OnInit() = 0;
	virtual void OnShutdown() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnGuiRender();
	virtual void OnEvent(const Event& event);

	void PushLayer(Shared<Layer> layer);
	void PushOverlay(Shared<Layer> overlay);
	void PopLayer(int count = 1);
	void PopOverlay(int count = 1);
	void EraseLayer(Shared<Layer> layer);
	void EraseOverlay(Shared<Layer> overlay);

	Shared<Window>& GetWindow() { return _window; }

	static App& Get() { return *s_Instance; }

	static String GetConfigurationName();
	static String GetPlatformName();

private:
	void Frame();

	bool OnWindowClose(const WindowCloseEvent& event);

	void RunSplashScreen();

protected:
	Shared<BatchLoader> _preLoader;

private:
	Unique<Renderer> _renderer;
	Unique<SceneRenderer> _sceneRenderer;

	Shared<Window> _window;
	Unique<Gui> _gui;

	bool _running = true, _minimized = false;
	LayerStack _layerStack;
	Mutex _finalPreloaderMessageMutex;

	Shared<Mouse> _mouse;
	Shared<Keyboard> _keyboard;

	Time _fpsTimer = Time::Zero;
	int _cachedFps = 0;
	Time _cachedSpf = Time::Zero;
	Time _storedFrametime = Time::Zero;
	int _storedFrameCount = 0;

	static App* s_Instance;
};

// Implemented by client
App* CreateApplication();
}
