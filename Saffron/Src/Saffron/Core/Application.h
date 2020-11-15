#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Project.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Core/LayerStack.h"
#include "Saffron/Core/Window.h"

namespace Se
{
class Application
{
	friend class ApplicationSerializer;

public:
	struct Properties
	{
		String Name;
		Uint32 WindowWidth, WindowHeight;
	};

public:
	Application(const Properties &properties = { "Saffron Engine", 1280, 720 });
	virtual ~Application();

	void Run();
	void Exit();

	virtual void OnInit() {}
	virtual void OnShutdown() {}
	virtual void OnUpdate() {}
	virtual void OnEvent(const Event &event);

	void PushLayer(Shared<Layer> layer);
	void PushOverlay(Shared<Layer> overlay);
	void PopLayer(int count = 1);
	void PopOverlay(int count = 1);
	void EraseLayer(Shared<Layer> layer);
	void EraseOverlay(Shared<Layer> overlay);

	void RenderGui();

	Window &GetWindow() { return *m_Window; }

	void AddProject(const Shared<Project> &project);
	void RemoveProject(const Shared<Project> &project);

	const ArrayList<Shared<Project>> &GetRecentProjectList() const;
	const Shared<Project> &GetActiveProject() const;
	void SetActiveProject(const Shared<Project> &project);

	static Application &Get() { return *s_Instance; }

	static String GetConfigurationName();
	static String GetPlatformName();

private:
	void RunSplashScreen();

	bool OnWindowClose(const WindowCloseEvent &event);

protected:
	Shared<BatchLoader> m_PreLoader;

private:
	Shared<Window> m_Window;
	bool m_Running = true, m_Minimized = false;
	LayerStack m_LayerStack;
	Mutex m_FinalPreloaderMessageMutex;

	mutable ArrayList<Shared<Project>> m_RecentProjectList;
	Shared<Project> m_ActiveProject = nullptr;

	Time ts;

	static Application *s_Instance;
};

// Implemented by client
Application *CreateApplication();

}