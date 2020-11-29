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

	void PushLayer(std::shared_ptr<Layer> layer);
	void PushOverlay(std::shared_ptr<Layer> overlay);
	void PopLayer(int count = 1);
	void PopOverlay(int count = 1);
	void EraseLayer(std::shared_ptr<Layer> layer);
	void EraseOverlay(std::shared_ptr<Layer> overlay);

	void RenderGui();

	Window &GetWindow() { return *m_Window; }

	void AddProject(const std::shared_ptr<Project> &project);
	void RemoveProject(const std::shared_ptr<Project> &project);

	const ArrayList<std::shared_ptr<Project>> &GetRecentProjectList() const;
	const std::shared_ptr<Project> &GetActiveProject() const;
	void SetActiveProject(const std::shared_ptr<Project> &project);

	static Application &Get() { return *s_Instance; }

	static String GetConfigurationName();
	static String GetPlatformName();

private:
	void RunSplashScreen();

	bool OnWindowClose(const WindowCloseEvent &event);

protected:
	std::shared_ptr<BatchLoader> m_PreLoader;

private:
	std::shared_ptr<Window> m_Window;
	bool m_Running = true, m_Minimized = false;
	LayerStack m_LayerStack;
	Mutex m_FinalPreloaderMessageMutex;

	mutable ArrayList<std::shared_ptr<Project>> m_RecentProjectList;
	std::shared_ptr<Project> m_ActiveProject = nullptr;

	Time ts;

	static Application *s_Instance;
};

// Implemented by client
Application *CreateApplication();

}