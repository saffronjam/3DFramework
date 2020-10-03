#include "SaffronPCH.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Core/Events/KeyboardEvent.h"
#include "Saffron/Core/Events/MouseEvent.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Platform/Windows/WindowsWindow.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/System/ScopedLock.h"

namespace Se
{

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char *description)
{
	SE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

WindowsWindow::WindowsWindow(const Properties &props)
	:
	Window(props),
	m_NativeWindow(nullptr),
	m_VSync(false)
{

	static std::mutex mutex;
	ScopedLock lock(mutex);

	// Initialize GLFW
	if ( !s_GLFWInitialized )
	{
		const int success = glfwInit();
		SE_CORE_ASSERT(success, "Failed to initialize GLFW");
		glfwSetErrorCallback(GLFWErrorCallback);
		s_GLFWInitialized = true;
	}

	// Create GLFW Window
	{
#ifdef SE_DEBUG
		if ( RendererAPI::Current() == RendererAPI::Type::OpenGL )
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		m_NativeWindow = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height),
										  m_Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_NativeWindow);
	}

	// Initialize glad
	{
		const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		SE_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	SE_CORE_INFO("Creating Window \"{0}\" ({1:d}x{2:d})", m_Title, m_Width, m_Height);

	SetupGLFWCallbacks();

	// Initialization events
	{
		double x, y;
		glfwGetCursorPos(m_NativeWindow, &x, &y);
		PushEvent<MouseMoveEvent>(glm::vec2(x, y));
	}
	{
		glfwSetWindowPos(m_NativeWindow, static_cast<int>(m_Position.x), static_cast<int>(m_Position.y));
	}

	WindowsWindow::SetVSync(true);

	m_ImGuiMouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);   // FIXME: GLFW doesn't have this.
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
	m_ImGuiMouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

}

WindowsWindow::~WindowsWindow()
{
	SE_PROFILE_FUNCTION();

	glfwDestroyWindow(static_cast<GLFWwindow *>(WindowsWindow::GetNativeWindow()));
}

void WindowsWindow::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_NativeWindow);

	const ImGuiMouseCursor ImGuiCursor = ImGui::GetMouseCursor();
	glfwSetCursor(m_NativeWindow, m_ImGuiMouseCursors[ImGuiCursor] ? m_ImGuiMouseCursors[ImGuiCursor] : m_ImGuiMouseCursors[ImGuiMouseCursor_Arrow]);
	glfwSetInputMode(m_NativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowsWindow::OnEvent(const Event &event)
{
	SE_PROFILE_FUNCTION();

	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizeEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnResize));
	dispatcher.Try<WindowMoveEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnMove));
	dispatcher.Try<WindowGainFocusEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnGainFocus));
	dispatcher.Try<WindowLostFocusEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnLostFocus));
	dispatcher.Try<WindowCloseEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnClose));
}

void WindowsWindow::Close()
{
	SE_PROFILE_FUNCTION();

	glfwSetWindowShouldClose(m_NativeWindow, GLFW_TRUE);
}

void WindowsWindow::Focus()
{
	SE_PROFILE_FUNCTION();

	glfwRequestWindowAttention(m_NativeWindow);
}

void *WindowsWindow::GetNativeWindow() const
{
	return m_NativeWindow;
}

void WindowsWindow::SetTitle(std::string title)
{
	SE_PROFILE_FUNCTION();

	m_Title = std::move(title);
	glfwSetWindowTitle(m_NativeWindow, m_Title.c_str());
}

void WindowsWindow::SetVSync(bool enabled)
{
	SE_PROFILE_FUNCTION();

	glfwSwapInterval(static_cast<int>(m_VSync = enabled));
}

bool WindowsWindow::IsVSync() const
{
	return m_VSync;
}

bool WindowsWindow::IsMinimized() const
{
	return m_Width == 0 || m_Height == 0;
}

bool WindowsWindow::OnResize(const WindowResizeEvent &event)
{
	m_Width = event.GetWidth();
	m_Height = event.GetHeight();

	auto *instance = this;
	Renderer::Submit([=]() { glViewport(0, 0, instance->m_Width, instance->m_Height); });
	auto &fbs = FramebufferPool::GetGlobal()->GetAll();
	for ( auto &fb : fbs )
		fb->Resize(instance->m_Width, instance->m_Height);

	return false;
}

bool WindowsWindow::OnMove(const WindowMoveEvent &event)
{
	m_Position = event.GetPosition();
	return false;
}

bool WindowsWindow::OnGainFocus(const WindowGainFocusEvent &event)
{
	return false;
}

bool WindowsWindow::OnLostFocus(const WindowLostFocusEvent &event)
{
	return false;
}

bool WindowsWindow::OnClose(const WindowCloseEvent &event)
{
	return false;
}

void WindowsWindow::SetupGLFWCallbacks()
{
	SE_PROFILE_FUNCTION();

	glfwSetWindowUserPointer(m_NativeWindow, this);

	// ----- Keyboard events -----
	glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods)
					   {
						   auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
						   if ( action == GLFW_PRESS )
							   pWnd->PushEvent<KeyboardPressEvent>(static_cast<KeyCode>(key));
						   else if ( action == GLFW_RELEASE )
							   pWnd->PushEvent<KeyboardReleaseEvent>(static_cast<KeyCode>(key));
						   else if ( action == GLFW_REPEAT )
							   pWnd->PushEvent<KeyboardRepeatEvent>(static_cast<KeyCode>(key));
					   });

	// ----- Mouse events -----
	glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow *window, int button, int action, int mods)
							   {
								   auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								   if ( action == GLFW_PRESS )
									   pWnd->PushEvent<MousePressEvent>(static_cast<MouseButtonCode>(button));
								   else if ( action == GLFW_RELEASE )
									   pWnd->PushEvent<MouseReleaseEvent>(static_cast<MouseButtonCode>(button));
							   });
	glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow *window, double xoffset, double yoffset)
						  {
							  auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
							  pWnd->PushEvent<MouseScrollEvent>(static_cast<float>(xoffset), static_cast<float>(yoffset));
						  });
	glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow *window, double xpos, double ypos)
							 {
								 auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								 pWnd->PushEvent<MouseMoveEvent>(glm::vec2(xpos, ypos));
							 });
	glfwSetCursorEnterCallback(m_NativeWindow, [](GLFWwindow *window, int enter)
							   {
								   auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								   if ( enter == GLFW_TRUE )
									   pWnd->PushEvent<MouseEnterEvent>();
								   else
									   pWnd->PushEvent<MouseLeaveEvent>();
							   });

	// ----- Window events -----
	glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow *window, int width, int height)
							  {
								  auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								  pWnd->PushEvent<WindowResizeEvent>(width, height);
							  });
	glfwSetWindowPosCallback(m_NativeWindow, [](GLFWwindow *window, int xpos, int ypos)
							 {
								 auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								 pWnd->PushEvent<WindowMoveEvent>(glm::vec2(xpos, ypos));
							 });
	glfwSetWindowFocusCallback(m_NativeWindow, [](GLFWwindow *window, int focus)
							   {
								   auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								   if ( focus == GLFW_TRUE )
									   pWnd->PushEvent<WindowGainFocusEvent>();
								   else
									   pWnd->PushEvent<WindowLostFocusEvent>();
							   });
	glfwSetWindowCloseCallback(m_NativeWindow, [](GLFWwindow *window)
							   {
								   auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								   pWnd->PushEvent<WindowCloseEvent>();
							   });
	glfwSetDropCallback(m_NativeWindow, [](GLFWwindow *window, int count, const char **paths)
						{
							auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
							std::vector<std::filesystem::path> filepaths(count);
							for ( int i = 0; i < count; i++ )
							{
								filepaths[i] = paths[i];
							}
							pWnd->PushEvent<WindowDropFilesEvent>(filepaths);
						});
}
}