#include "SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Platform/Windows/WindowsWindow.h"

#include "imgui.h"

namespace Se
{
static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description)
{
	Log::Warn("GLFW Error ({0}): {1}", error, description);
}

WindowsWindow::WindowsWindow(const WindowSpec& spec) :
	Window(spec),
	m_NativeWindow(nullptr),
	m_VSync(false)
{
	if (!s_GLFWInitialized)
	{
		const int success = glfwInit();
		Debug::Assert(success, "Failed to initialize GLFW");
		glfwSetErrorCallback(GLFWErrorCallback);
		s_GLFWInitialized = true;
	}

	m_NativeWindow = glfwCreateWindow(static_cast<int>(spec.Width), static_cast<int>(spec.Height), spec.Title.c_str(),
	                                  nullptr, nullptr);
	glfwMakeContextCurrent(m_NativeWindow);

	SetupGLFWCallbacks();

	// Initialization events
	double xD, yD;
	glfwGetCursorPos(m_NativeWindow, &xD, &yD);
	const int x = static_cast<int>(xD), y = static_cast<int>(yD);
	PushEvent({.Type = EventType::MouseMoved, .MouseMove = {x, y}});
	glfwSetWindowPos(m_NativeWindow, x, y);


	m_ImGuiMouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	// FIXME: GLFW doesn't have this.
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	// FIXME: GLFW doesn't have this.
	m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	// FIXME: GLFW doesn't have this.
	m_ImGuiMouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

WindowsWindow::~WindowsWindow()
{
	glfwDestroyWindow(static_cast<GLFWwindow*>(WindowsWindow::NativeHandle()));
}

void WindowsWindow::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_NativeWindow);

	const ImGuiMouseCursor ImGuiCursor = ImGui::GetMouseCursor();
	glfwSetCursor(m_NativeWindow, m_ImGuiMouseCursors[ImGuiCursor]
		                              ? m_ImGuiMouseCursors[ImGuiCursor]
		                              : m_ImGuiMouseCursors[ImGuiMouseCursor_Arrow]);
	glfwSetInputMode(m_NativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void* WindowsWindow::NativeHandle() const
{
	return m_NativeWindow;
}

void WindowsWindow::SetupGLFWCallbacks()
{
	glfwSetWindowUserPointer(m_NativeWindow, this);

	// ----- Keyboard events -----
	glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS) pWnd->PushEvent({.Type = EventType::KeyPressed, .Key = static_cast<KeyCode>(key)});
		else if (action == GLFW_RELEASE)
			pWnd->PushEvent({.Type = EventType::KeyReleased, .Key = {static_cast<KeyCode>(key), false, false, false},});
	});

	// ----- Mouse events -----
	glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* window, int button, int action, int mods)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS)
		{
			pWnd->PushEvent({
				.Type = EventType::MouseButtonPressed, .MouseButton = {static_cast<MouseButtonCode>(button), 0, 0}
			});
		}
		else if (action == GLFW_RELEASE)
		{
			pWnd->PushEvent({
				.Type = EventType::MouseButtonReleased, .MouseButton = {static_cast<MouseButtonCode>(button), 0, 0}
			});
		}
	});
	glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		pWnd->PushEvent(
			{.Type = EventType::MouseWheelScrolled, .MouseWheelScroll = {static_cast<float>(xoffset), 0, 0}});
	});
	glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* window, double xpos, double ypos)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		pWnd->PushEvent({.Type = EventType::MouseMoved, .MouseMove = {static_cast<int>(xpos), static_cast<int>(ypos)}});
	});
	glfwSetCursorEnterCallback(m_NativeWindow, [](GLFWwindow* window, int enter)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		if (enter == GLFW_TRUE)
		{
			pWnd->PushEvent({.Type = EventType::MouseEntered, .MouseMove = {0, 0}});
		}
		else
		{
			pWnd->PushEvent({.Type = EventType::MouseLeft, .MouseMove = {0, 0}});
		}
	});

	// ----- Window events -----
	glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height)
	{
	});
	glfwSetWindowPosCallback(m_NativeWindow, [](GLFWwindow* window, int xpos, int ypos)
	{
	});
	glfwSetWindowFocusCallback(m_NativeWindow, [](GLFWwindow* window, int focus)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		if (focus == GLFW_TRUE)
		{
			pWnd->PushEvent({.Type = EventType::LostFocus});
		}
		else
		{
			pWnd->PushEvent({.Type = EventType::LostFocus});
		}
	});
	glfwSetWindowCloseCallback(m_NativeWindow, [](GLFWwindow* window)
	{
		auto* pWnd = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
		pWnd->PushEvent({.Type = EventType::Closed});
	});
}

void WindowsWindow::Restore()
{
	glfwRestoreWindow(m_NativeWindow);
	m_Minimized = false;
	m_Maximized = false;
}
}
