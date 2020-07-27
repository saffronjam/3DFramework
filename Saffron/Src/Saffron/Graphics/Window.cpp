
#include <Glad/glad.h>
#include <GLFW/glfw3.h>

#include "Saffron/Graphics/Window.h"
#include "Saffron/Core/Engine.h"
#include "Saffron/Event/KeyboardEvent.h"
#include "Saffron/Event/MouseEvent.h"
#include "Saffron/Event/WindowEvent.h"
#include "Saffron/GL/GLCheck.h"
#include "Saffron/System/ScopedLock.h"


//#include "imgui/imgui.h"
//#include "BindableCodex.h"

namespace Saffron
{

bool Window::m_sInitialized = false;
bool Window::m_sAnyActiveContext = false;

Window::Window(std::string title, int width, int height, bool activeContext)
	:
	m_title(std::move(title)),
	m_width(width),
	m_height(height),
	m_position(100.0f, 100.0f),
	m_hasFocus(false),
	kbd(*this),
	mouse(*this)
{
	static auto *failMessage = "Failed to initialize {0}";
	static auto *successMessage = "Successfully initialized {0}";
	static std::mutex mutex;
	ScopedLock lock(mutex);

	// Initialize GLFW
	if ( !m_sInitialized )
	{
		const int success = glfwInit();
		SE_CORE_ASSERT(success, failMessage, "GLFW");
		if ( success )
		{
			SE_CORE_INFO(successMessage, "GLFW");
		}
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	m_nativeWindowHandle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
	if ( activeContext || !m_sAnyActiveContext )
	{
		glfwMakeContextCurrent(GetCoreWindow());
		m_sAnyActiveContext = true;
	}
	SetupGLFWCallbacks();

	// Initialize Glad
	if ( !m_sInitialized )
	{
		const int success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		SE_CORE_ASSERT(success, failMessage, "Glad");
		if ( success )
		{
			SE_CORE_INFO(successMessage, "Glad");
		}
	}
	glCheck(glEnable(GL_DEPTH_TEST));
	glCheck(glEnable(GL_CULL_FACE));

	m_sInitialized = true;

	SE_CORE_INFO("Creating Window \"{0}\" ({1:d}x{2:d})", m_title, m_width, m_height);

	// Initialization events
	{
		double x, y;
		glfwGetCursorPos(GetCoreWindow(), &x, &y);
		PushEvent<MouseMoveEvent>(glm::vec2(x, y));
	}
	{
		glfwSetWindowPos(GetCoreWindow(), static_cast<int>(m_position.x), static_cast<int>(m_position.y));
	}
	RequestFocus();
}

Window::~Window()
{
	//Bind::Codex::Destroy();
	glfwDestroyWindow(m_nativeWindowHandle);
}

void Window::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(GetCoreWindow());
	glCheck(glClearColor(1, 0, 1, 1));
	glCheck(glClear(GL_COLOR_BUFFER_BIT));
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(m_nativeWindowHandle);
}

void Window::RequestClose()
{
	glfwSetWindowShouldClose(m_nativeWindowHandle, GLFW_TRUE);
}

void Window::RequestFocus()
{
	glfwRequestWindowAttention(GetCoreWindow());
}

void Window::HandleBufferedEvents()
{
	glfwPollEvents();
	for ( auto &event : m_events )
	{
		for ( auto &handler : m_handlers )
		{
			handler->OnEvent(event);
		}
	}
	m_events.clear();
}

void Window::AddEventHandler(EventHandler *handler)
{
	m_handlers.emplace(handler);
}

void Window::OnEvent(const Event::Ptr event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizeEvent>(EVENT_FN(Window::OnResize));
	dispatcher.Try<WindowMoveEvent>(EVENT_FN(Window::OnMove));
	dispatcher.Try<WindowGainFocusEvent>(EVENT_FN(Window::OnGainFocus));
	dispatcher.Try<WindowLostFocusEvent>(EVENT_FN(Window::OnLostFocus));
	dispatcher.Try<WindowCloseEvent>(EVENT_FN(Window::OnClose));
}

GLFWwindow *Window::GetCoreWindow() const
{
	return m_nativeWindowHandle;
}

const glm::vec2 &Window::GetPosition() const
{
	return m_position;
}

int Window::GetWidth() const
{
	return m_width;
}

int Window::GetHeight() const
{
	return m_height;
}

bool Window::HasFocus() const
{
	return m_hasFocus;
}

void Window::SetTitle(std::string title)
{
	m_title = std::move(title);
	glfwSetWindowTitle(m_nativeWindowHandle, m_title.c_str());
}

void Window::SetVSync(bool enabled)
{
	glfwSwapInterval(static_cast<int>(enabled));
}

void Window::SetupGLFWCallbacks()
{
	glfwSetWindowUserPointer(m_nativeWindowHandle, this);
	glfwSetKeyCallback(m_nativeWindowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods)
					   {
						   auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
						   if ( action == GLFW_PRESS )
							   pWnd->PushEvent<KeyboardPressEvent>(static_cast<KeyCode>(key));
						   else if ( action == GLFW_RELEASE )
							   pWnd->PushEvent<KeyboardReleaseEvent>(static_cast<KeyCode>(key));
						   else if ( action == GLFW_REPEAT )
							   pWnd->PushEvent<KeyboardRepeatEvent>(static_cast<KeyCode>(key));
					   });
	glfwSetMouseButtonCallback(m_nativeWindowHandle, [](GLFWwindow *window, int button, int action, int mods)
							   {
								   auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								   if ( action == GLFW_PRESS )
									   pWnd->PushEvent<MousePressEvent>(static_cast<ButtonCode>(button));
								   else if ( action == GLFW_RELEASE )
									   pWnd->PushEvent<MouseReleaseEvent>(static_cast<ButtonCode>(button));
							   });
	glfwSetCursorPosCallback(m_nativeWindowHandle, [](GLFWwindow *window, double xpos, double ypos)
							 {
								 auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								 pWnd->PushEvent<MouseMoveEvent>(glm::vec2(xpos, ypos));
							 });
	glfwSetCursorEnterCallback(m_nativeWindowHandle, [](GLFWwindow *window, int enter)
							   {
								   auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								   if ( enter == GLFW_TRUE )
									   pWnd->PushEvent<MouseEnterEvent>(pWnd->mouse.GetPosition());
								   else
									   pWnd->PushEvent<MouseLeaveEvent>(pWnd->mouse.GetPosition());
							   });
	glfwSetWindowSizeCallback(m_nativeWindowHandle, [](GLFWwindow *window, int width, int height)
							  {
								  auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								  pWnd->PushEvent<WindowResizeEvent>(width, height);
							  });
	glfwSetWindowPosCallback(m_nativeWindowHandle, [](GLFWwindow *window, int xpos, int ypos)
							 {
								 auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								 pWnd->PushEvent<WindowMoveEvent>(glm::vec2(xpos, ypos));
							 });
	glfwSetWindowFocusCallback(m_nativeWindowHandle, [](GLFWwindow *window, int focus)
							   {
								   auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								   if ( focus == GLFW_TRUE )
									   pWnd->PushEvent<WindowGainFocusEvent>();
								   else
									   pWnd->PushEvent<WindowLostFocusEvent>();
							   });
	glfwSetWindowCloseCallback(m_nativeWindowHandle, [](GLFWwindow *window)
							   {
								   auto *pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
								   pWnd->PushEvent<WindowCloseEvent>();
							   });
}

void Window::OnResize(const WindowResizeEvent::Ptr event)
{
	m_width = event->GetWidth();
	m_height = event->GetHeight();
}

void Window::OnMove(const WindowMoveEvent::Ptr event)
{
	m_position = event->GetPosition();
}

void Window::OnGainFocus(const WindowGainFocusEvent::Ptr event)
{
}

void Window::OnLostFocus(const WindowLostFocusEvent::Ptr event)
{
}

void Window::OnClose(const WindowCloseEvent::Ptr event)
{
}

}
