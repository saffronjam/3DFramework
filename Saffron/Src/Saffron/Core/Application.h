#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/WindowEvent.h"
#include "Saffron/Core/DateTime.h"
#include "Saffron/Core/LayerStack.h"
#include "Saffron/Core/Window.h"
#include "Saffron/Renderer/Texture.h"

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

	struct Project
	{
		UUID UUID;
		String Name;
		Filepath SceneFilepath;
		Shared<Texture2D> PreviewTexture;
		DateTime LastOpened;

		bool operator==(const Project &rhs) const { return UUID == rhs.UUID; }
		bool operator<(const Project &rhs) const { return LastOpened < rhs.LastOpened; }
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

	void PushLayer(Layer *layer);
	void PushOverlay(Layer *layer);
	void RenderGui();

	Window &GetWindow() { return *m_Window; }
	const ArrayList<Project> &GetProjectList() const;
	const Project &GetActiveProject() const;
	void SetActiveProject(const Project &project);

	static Application &Get() { return *s_Instance; }

	static String GetConfigurationName();
	static String GetPlatformName();

private:
	bool OnWindowClose(const WindowCloseEvent &event);

private:
	Shared<Window> m_Window;
	bool m_Running = true, m_Minimized = false;
	LayerStack m_LayerStack;
	Mutex m_FinalPreloaderMessageMutex;
	Shared<BatchLoader> m_PreLoader;

	mutable ArrayList<Project> m_ProjectList;
	Project *m_ActiveProject = nullptr;

	Time ts;

	static Application *s_Instance;
};

// Implemented by client
Application *CreateApplication();

}