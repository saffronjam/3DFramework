#include "Saffron/SaffronPCH.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Platform/Windows/WindowsWindow.h"
#include "Saffron/Event/KeyboardEvent.h"
#include "Saffron/Event/MouseEvent.h"
#include "Saffron/GL/GLCheck.h"
#include "Saffron/System/ScopedLock.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Se
{

bool WindowsWindow::m_sInitialized = false;

WindowsWindow::WindowsWindow(const WindowProps &props)
	:
	m_NativeWindow(nullptr),
	m_VSync(false)
{
	static const auto *failMessage = "Failed to initialize {0}";
	static const auto *successMessage = "Successfully initialized {0}";

	m_Title = props.Title;
	m_Width = props.Width;
	m_Height = props.Height;
	m_Position = props.Position;

	static std::mutex mutex;
	ScopedLock lock(mutex);

	// Initialize GLFW
	if ( !m_sInitialized )
	{
		const auto success = glfwInit();
		SE_CORE_ASSERT(success, failMessage, "GLFW");
		if ( success )
		{
			SE_CORE_INFO(successMessage, "GLFW");
		}
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	m_NativeWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
	SetupGLFWCallbacks();

	// Initialize Glad
	if ( !m_sInitialized )
	{
		const auto success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		SE_CORE_ASSERT(success, failMessage, "Glad");
		if ( success )
		{
			SE_CORE_INFO(successMessage, "Glad");
		}
	}
	glCheck(glEnable(GL_DEPTH_TEST));
	glCheck(glEnable(GL_CULL_FACE));

	m_sInitialized = true;

	SE_CORE_INFO("Creating Window \"{0}\" ({1:d}x{2:d})", m_Title, m_Width, m_Height);

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
	glfwDestroyWindow(static_cast<GLFWwindow *>(WindowsWindow::GetNativeWindow()));
}

void WindowsWindow::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_NativeWindow);
	glCheck(glClearColor(1, 0, 1, 1));
	glCheck(glClear(GL_COLOR_BUFFER_BIT));
}

void WindowsWindow::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizeEvent>(SE_EVENT_FN(WindowsWindow::OnResize));
	dispatcher.Try<WindowMoveEvent>(SE_EVENT_FN(WindowsWindow::OnMove));
	dispatcher.Try<WindowGainFocusEvent>(SE_EVENT_FN(WindowsWindow::OnGainFocus));
	dispatcher.Try<WindowLostFocusEvent>(SE_EVENT_FN(WindowsWindow::OnLostFocus));
	dispatcher.Try<WindowCloseEvent>(SE_EVENT_FN(WindowsWindow::OnClose));
}

void WindowsWindow::Close()
{
	glfwSetWindowShouldClose(m_NativeWindow, GLFW_TRUE);

}

void WindowsWindow::Focus()
{
	glfwRequestWindowAttention(m_NativeWindow);
}

void *WindowsWindow::GetNativeWindow() const
{
	return m_NativeWindow;
}

void WindowsWindow::SetTitle(std::string title)
{
	m_Title = std::move(title);
	glfwSetWindowTitle(m_NativeWindow, m_Title.c_str());
}

void WindowsWindow::SetVSync(bool enabled)
{
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
	glfwSetWindowUserPointer(m_NativeWindow, this);
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
	glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow *window, int button, int action, int mods)
							   {
								   auto *pWnd = static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
								   if ( action == GLFW_PRESS )
									   pWnd->PushEvent<MousePressEvent>(static_cast<ButtonCode>(button));
								   else if ( action == GLFW_RELEASE )
									   pWnd->PushEvent<MouseReleaseEvent>(static_cast<ButtonCode>(button));
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
