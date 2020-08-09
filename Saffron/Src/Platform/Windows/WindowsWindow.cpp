#include "Saffron/SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Renderer/Renderer.h"
#include "Saffron/System/ScopedLock.h"

#include "Saffron/Core/Event/KeyboardEvent.h"
#include "Saffron/Core/Event/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Se
{

Uint16 WindowsWindow::m_sGLFWWindowCount = 0u;

static void GLFWErrorCallback(int error, const char *description)
{
	SE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

WindowsWindow::WindowsWindow(const WindowProps &props)
	:
	m_NativeWindow(nullptr),
	m_VSync(false)
{
	SE_PROFILE_FUNCTION();

	m_Title = props.Title;
	m_Width = props.Width;
	m_Height = props.Height;
	m_Position = props.Position;

	static std::mutex mutex;
	ScopedLock lock(mutex);

	// Initialize GLFW
	if ( m_sGLFWWindowCount == 0 )
	{
		SE_PROFILE_SCOPE("glfwInit");

		const int success = glfwInit();
		SE_CORE_ASSERT(success, "Failed to initialize GLFW");
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	{
		SE_PROFILE_SCOPE("glfwCreateWindow");

#ifdef SE_DEBUG
		if ( Renderer::GetAPI() == RendererAPI::API::OpenGL )
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		m_NativeWindow = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Title.c_str(), nullptr, nullptr);
		++m_sGLFWWindowCount;
	}

	m_Context = GraphicsContext::Create(m_NativeWindow);

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
}

WindowsWindow::~WindowsWindow()
{
	SE_PROFILE_FUNCTION();

	glfwDestroyWindow(static_cast<GLFWwindow *>(WindowsWindow::GetNativeWindow()));
}

void WindowsWindow::OnUpdate()
{
	SE_PROFILE_FUNCTION();

	glfwPollEvents();
	m_Context->SwapBuffers();
}

void WindowsWindow::OnEvent(const Event &event)
{
	SE_PROFILE_FUNCTION();

	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(WindowsWindow::OnResize));
	dispatcher.Try<WindowMoveEvent>(SE_EVENT_FN(WindowsWindow::OnMove));
	dispatcher.Try<WindowGainFocusEvent>(SE_EVENT_FN(WindowsWindow::OnGainFocus));
	dispatcher.Try<WindowLostFocusEvent>(SE_EVENT_FN(WindowsWindow::OnLostFocus));
	dispatcher.Try<WindowCloseEvent>(SE_EVENT_FN(WindowsWindow::OnClose));
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

void WindowsWindow::OnResize(const WindowResizeEvent &event)
{
	m_Width = event.GetWidth();
	m_Height = event.GetHeight();
}

void WindowsWindow::OnMove(const WindowMoveEvent &event)
{
	m_Position = event.GetPosition();
}

void WindowsWindow::OnGainFocus(const WindowGainFocusEvent &event)
{
}

void WindowsWindow::OnLostFocus(const WindowLostFocusEvent &event)
{
}

void WindowsWindow::OnClose(const WindowCloseEvent &event)
{
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
									   pWnd->PushEvent<MousePressEvent>(static_cast<ButtonCode>(button));
								   else if ( action == GLFW_RELEASE )
									   pWnd->PushEvent<MouseReleaseEvent>(static_cast<ButtonCode>(button));
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
}
}
