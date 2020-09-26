#include "SaffronPCH.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <Commdlg.h>
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Renderer/Framebuffer.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Script/ScriptEngine.h"

namespace Se {

Application *Application::s_Instance = nullptr;

Application::Application(const Properties &properties)
{
	SE_ASSERT(!s_Instance, "Application already exist");
	s_Instance = this;

	m_Window = Window::Create(Window::Properties(properties.Name, properties.WindowWidth, properties.WindowHeight));
	m_Window->SetEventCallback(SE_BIND_EVENT_FN(OnEvent));
	m_Window->SetVSync(true);

	m_GuiLayer = new GuiLayer("ImGui");
	PushOverlay(m_GuiLayer);

	ScriptEngine::Init("Assets/Scripts/ExampleApp.dll");

	Renderer::Init();
	Renderer::WaitAndRender();

	ts = Timer::GlobalMark();
}

Application::~Application()
{
	ScriptEngine::Shutdown();
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

void Application::RenderGui()
{
	m_GuiLayer->Begin();

	ImGui::Begin("Renderer");
	auto &caps = RendererAPI::GetCapabilities();
	ImGui::Text("Vendor: %s", caps.Vendor.c_str());
	ImGui::Text("Renderer: %s", caps.Renderer.c_str());
	ImGui::Text("Version: %s", caps.Version.c_str());
	ImGui::Text("Frame Time: %.2fms\n", ts.ms());
	ImGui::End();

	for ( Layer *layer : m_LayerStack )
		layer->OnImGuiRender();

	m_GuiLayer->End();
}

void Application::Run()
{
	OnInit();
	while ( m_Running )
	{
		m_Window->HandleBufferedEvents();
		if ( !m_Minimized )
		{
			for ( Layer *layer : m_LayerStack )
				layer->OnUpdate(ts);

			// Render ImGui on render thread
			Application *app = this;
			Renderer::Submit([app]() { app->RenderGui(); });

			Renderer::WaitAndRender();
		}
		m_Window->OnUpdate();

		ts = Timer::GlobalMark();
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

std::filesystem::path Application::OpenFile(const char *filter) const
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

std::filesystem::path Application::SaveFile(const char *filter) const
{
	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(m_Window->GetNativeWindow()));
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = "ssc";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT;

	if ( GetSaveFileNameA(&ofn) == TRUE )
	{
		return ofn.lpstrFile;
	}
	return std::filesystem::path();
}

float Application::GetTime() const
{
	return static_cast<float>(glfwGetTime());
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