#include "Saffron/SaffronPCH.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <Commdlg.h>
#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Renderer/Framebuffer.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Script/ScriptEngine.h"


namespace Se
{

Application *Application::m_sInstance = nullptr;

Application::Application(const Properties &properties)
{
	SE_ASSERT(!m_sInstance, "Application already exist");
	m_sInstance = this;

	m_Window = Window::Create();

	m_Window->SetEventCallback(SE_EVENT_FN(Application::OnEvent));
	m_Window->SetVSync(true);

	m_ImGuiLayer = new ImGuiLayer;
	PushLayer(m_ImGuiLayer);

	ScriptEngine::Init("Assets/scripts/ExampleApp.dll");
	Renderer::Init();
	Renderer::WaitAndRender();

	dt = m_AppTimer.Mark();
}

Application::~Application()
{
	ScriptEngine::Shutdown();
	delete m_ImGuiLayer;
}


void Application::Run()
{
	OnInit();
	while ( m_Running )
	{
		if ( !m_Minimized )
		{
			for ( auto *layer : m_LayerStack )
				layer->OnUpdate(dt);

			// Render ImGui on render thread
			Application *app = this;
			Renderer::Submit([app]() { app->RenderImGui(); });

			Renderer::WaitAndRender();
		}
		m_Window->OnUpdate();

		dt = m_AppTimer.Mark();
	}
	OnShutdown();
}

void Application::Close()
{
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

void Application::RenderImGui()
{
	m_ImGuiLayer->Begin();

	ImGui::Begin("Renderer");
	auto &caps = RendererAPI::GetCapabilities();
	ImGui::Text("Vendor: %s", caps.Vendor.c_str());
	ImGui::Text("Renderer: %s", caps.Renderer.c_str());
	ImGui::Text("Version: %s", caps.Version.c_str());
	ImGui::Text("Frame Time: %.2fms\n", dt.ms());
	ImGui::End();

	for ( Layer *layer : m_LayerStack )
		layer->OnImGuiRender();

	m_ImGuiLayer->End();
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

std::string Application::OpenFile(const char *filter) const
{
	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(m_Window->GetNativeWindow()));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if ( GetOpenFileNameA(&ofn) == TRUE )
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

std::string Application::SaveFile(const char *filter) const
{
	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(m_Window->GetNativeWindow()));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if ( GetSaveFileNameA(&ofn) == TRUE )
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

void Application::OnEvent(const Event &event)
{
	SE_PROFILE_FUNCTION();

	// TODO: Optimize this if event is handled?

	// Application events
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowCloseEvent>(SE_EVENT_FN(Application::OnWindowClose));
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(Application::OnWindowResize));

	// Window events
	m_Window->OnEvent(event);

	// Keyboard events
	m_Keyboard.OnEvent(event);

	// Mouse events
	m_Mouse.OnEvent(event);

	// Layer events
	for ( auto &layer : m_LayerStack )
	{
		layer->OnEvent(event);
	}
}

Ref<Window> &Application::GetWindow()
{
	return m_Window;
}

const Ref<Window> &Application::GetWindow() const
{
	return m_Window;
}

void Application::OnWindowClose(const WindowCloseEvent &event)
{
	m_Running = false;
}

void Application::OnWindowResize(const WindowResizeEvent &event)
{
	const int width = event.GetWidth(), height = event.GetHeight();
	if ( width == 0 || height == 0 )
	{
		m_Minimized = true;
		return;
	}
	m_Minimized = false;
	Renderer::Submit([=]() { glViewport(0, 0, width, height); });
	auto &fbs = FramebufferPool::GetGlobal()->GetAll();
	for ( auto &fb : fbs )
		fb->Resize(width, height);
}

}
